#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HTTPレスポンスを構築する
 *
 * 成功時は 0 を返し、*out にHTTP レスポンス文字列を格納する。 失敗時は -1を返す。
 *
 * @param out   出力文字列へのポインタ（成功時に malloc した文字列が入る）
 * @param body  レスポンスボディの文字列
 * @return 0 on success, -1 on error
 */
int build_resp_dup(char **out, const char *body);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* HTTP_RESPONSE_H */
