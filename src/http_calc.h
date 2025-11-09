#ifndef HTTP_CALC_H
#define HTTP_CALC_H

#include "http_request.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RequestLine を検証し、/calc?query=A+Bを評価して計算した文字列を返す。
 *
 * 成功時は 0 を返し、*out にNUL 終端文字列を格納する。 失敗時は -1
 * を返し、errno に理由を設定する。
 *
 * @param request_line  入力の RequestLine へのポインタ
 * @param out           出力文字列へのポインタ（成功時に mallocした文字列が入る）
 * @return 0 on success, -1 on error (errno set)
 */
int calc_eval_request_line_dup(const RequestLine *request_line, char **out);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* HTTP_CALC_H */
