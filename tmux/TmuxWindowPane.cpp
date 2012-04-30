#include "tmux/TmuxWindowPane.h"
#include <iostream>
#include <sstream>

extern "C" void swap_screen(Terminal *, int, int, int);

TmuxWindowPane::TmuxWindowPane(TmuxGateway *gateway, GuiTerminalWindow *termwnd)
    : _gateway(gateway),
      _termWnd(termwnd),
      old_state(NULL)
{
}

TmuxWindowPane::~TmuxWindowPane()
{
    if (old_state)
        delete old_state;
}

int TmuxWindowPane::performCallback(tmux_cb_index_t index, string &response)
{
    qDebug("TMUX resp %d %s", index, response.c_str());
    switch(index) {
    case CB_NULL:
        return 0;
    case CB_LIST_WINDOWS:
        return 0;
    case CB_DUMP_TERM_STATE:
        return resp_hdlr_dump_term_state(response);
    case CB_DUMP_HISTORY:
        return resp_hdlr_dump_history(response);
    case CB_DUMP_HISTORY_ALT:
        return 0;
    default:
        return -1;
    }
}

int TmuxWindowPane::resp_hdlr_dump_term_state(string &response)
{
    string key;
    int n;
    istringstream iresp(response);

    old_state = new tmux_old_state_t();
    while (std::getline(iresp, key, '=')) {
        if (!key.compare("in_alternate_screen")) {
            iresp>>n;
            old_state->in_alt_screen = n;
        } else if (!key.compare("base_cursor_x")) {
            iresp>>n;
            old_state->base_cursor_x = n;
        } else if (!key.compare("base_cursor_y")) {
            iresp>>n;
            old_state->base_cursor_y = n;
        } else if (!key.compare("cursor_x")) {
            iresp>>n;
            old_state->cursor_x = n;
        } else if (!key.compare("cursor_y")) {
            iresp>>n;
            old_state->cursor_y = n;
        } else if (!key.compare("scroll_region_upper")) {
            iresp>>n;
            old_state->scroll_region_upper = n;
        } else if (!key.compare("scroll_region_lower")) {
            iresp>>n;
            old_state->scroll_region_lower = n;
        } else if (!key.compare("decsc_cursor_x")) {
            iresp>>n;
            old_state->decsc_cursor_x = n;
        } else if (!key.compare("decsc_cursor_y")) {
            iresp>>n;
            old_state->decsc_cursor_y = n;
        } else if (!key.compare("tabstops")) {
            iresp>>old_state->tabstops;
        } else if (!key.compare("insert_mode")) {
            iresp>>n;
            _termWnd->term->insert = n;
        } else if (!key.compare("wrap_mode")) {
            iresp>>n;
            _termWnd->term->wrap = n;
        }
        iresp.ignore(128, '\n');
    }
    if (old_state->in_alt_screen) {
        _termWnd->term->curs.x = old_state->base_cursor_x;
        _termWnd->term->curs.y = old_state->base_cursor_y;
        //swap_screen(_termWnd->term, TRUE, TRUE, TRUE);
    }
    _termWnd->term->curs.x = old_state->cursor_x;
    _termWnd->term->curs.y = old_state->cursor_y;
    _termWnd->term->marg_t = old_state->scroll_region_upper;
    _termWnd->term->marg_b = old_state->scroll_region_lower;
    return 0;
cu0:
    qDebug()<<"Error in dump_term_state";
    delete old_state;
    old_state = NULL;
    return -1;
}

extern "C" termline *newline(Terminal *term, int cols, int bce);
extern "C" unsigned char *compressline(termline *ldata);

int TmuxWindowPane::resp_hdlr_dump_history(string &response, bool is_alt)
{
    ostringstream fail_reason;
    istringstream iresp(response);
    string line, hist;
    int num_lines;
    tree234 *tree;
    termline *tline;
    struct termchar tchar;

    // make sure we start with clean slate
    assert(count234(_termWnd->term->scrollback) == 0);
    assert(count234(_termWnd->term->alt_screen) == _termWnd->term->rows);
    assert(count234(_termWnd->term->alt_screen) == _termWnd->term->rows);

    num_lines = std::count(response.begin(), response.end(), '\n');
    memset(&tchar, 0, sizeof(termchar));

    for (int cur_line = 0; std::getline(iresp, line); cur_line++) {
        bool softeol = false;
        char c;
        bool is_utf8 = false;
        string utf8str;
        int cur_pos = 0;

        if (is_alt) {
            tree = _termWnd->term->alt_screen;
            tline = newline(_termWnd->term, _termWnd->term->cols, FALSE);
            addpos234(tree, tline, cur_line);
        } else if (num_lines - cur_line <= _termWnd->term->rows) {
            tree = _termWnd->term->screen;
            tline = (termline*) index234(tree,
                                         _termWnd->term->rows-(num_lines-cur_line));
        } else {
            tree = _termWnd->term->scrollback;
            tline = newline(_termWnd->term, _termWnd->term->cols, FALSE);
        }

        if (line.length() > 0 && line.at(line.length()-1) == '+') {
            line.erase(line.length()-1);
            softeol = true;
        }
        istringstream iss(line);
        while ( iss>>c ) {
            if (c == ':') {
                if (is_utf8) {
                    fail_reason << "Malformed text no closing ']'";
                    goto cu0;
                }
                int values[4];
                for (int j = 0; j < 4; j++) {
                    char tmp;
                    iss>>std::hex>>values[j]>>tmp;
                    if (tmp != ',') {
                        fail_reason << "Malformed text after ':'' no ','";
                        goto cu0;
                    }
                }

                int attr, fg, bg;
                attr = values[0];
                fg = values[2]==8 ? ATTR_DEFFG : values[2]<<ATTR_FGSHIFT;
                bg = values[3]==8 ? ATTR_DEFBG : values[3]<<ATTR_BGSHIFT;
                tchar.attr = 0;
                tchar.attr |= fg;
                tchar.attr |= bg;

                if (attr & GRID_ATTR_BRIGHT)
                    tchar.attr |= ATTR_BOLD;
                if (attr & GRID_ATTR_UNDERSCORE)
                    tchar.attr |= ATTR_UNDER;
                if (attr & GRID_ATTR_BLINK)
                    tchar.attr |= ATTR_BLINK;
                if (attr & GRID_ATTR_REVERSE)
                    tchar.attr |= ATTR_REVERSE;

                qDebug()<<"attr, flags, fg, bg"<<values[0]<<values[1]<<values[2]<<values[3];
            } else if (c == '*') {
                if (is_utf8) {
                    fail_reason << "Malformed text no closing ']'";
                    goto cu0;
                }
                // We have a "*<number> " sequence.
                int repeats;
                char c2;
                iss>>std::dec>>std::noskipws>>repeats>>c2;
                if (c2 != ' ') {
                    fail_reason << "lacks a space after *n line:"<<line<<" pos:"<<iss.tellg();
                    goto cu0;
                }
                if (!iss) {
                    fail_reason << "lacks a number after * line:"<<line<<" pos:"<<iss.tellg();
                    goto cu0;
                }
                qDebug("repeats %d %x\n", repeats, tchar.chr);
                for (int j = 0; j < repeats - 1; j++) {
                    assert(cur_pos < tline->cols);
                    tline->chars[cur_pos++] = tchar;
                    hist.push_back((char)tchar.chr);
                }
            } else if (c == '[') {
                if (iss.rdbuf()->in_avail() < 3) {
                    fail_reason << "Malformed text after '[' in history";
                    goto cu0;
                }
                is_utf8 = true;
                utf8str.clear();
            } else if (c == ']') {
                if (!is_utf8) {
                    fail_reason << "']' without '[' in history";
                    goto cu0;
                }
                is_utf8 = false;
            } else if (is_hex_char(c) && is_hex_char((char)iss.peek())) {
                char c2;
                iss>>c2;
                c2 = hex_to_char(c)<<4 | hex_to_char(c2);
                if (is_utf8) {
                    utf8str.push_back(c2);
                } else {
                    tchar.chr = c2;
                }
                assert(cur_pos < tline->cols);
                tline->chars[cur_pos++] = tchar;
                hist.push_back(c2);
            } else {
                fail_reason << "char not expected "<<c<<" line:"<<line<<" pos:"<<iss.tellg();
                goto cu0;
            }
        }

        if (tree == _termWnd->term->scrollback) {
            addpos234(tree, compressline(tline), cur_line);
            sfree(tline);
            tline = NULL;
        }

        qDebug()<<"history"<<hist.c_str();
        hist.clear();
    }
    return 0;
cu0:
    if (tree == _termWnd->term->scrollback)
        sfree(tline);

    qDebug("Malformed history dump response reason: %s\n", fail_reason.str().c_str());
    return -1;
}
