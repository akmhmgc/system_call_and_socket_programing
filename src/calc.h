#ifndef CALC_H
#define CALC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 式を評価して加算結果を返す (例: "1+2" -> 3)
 *
 * @param expression 評価する式（例: "1+2"）
 * @param out 計算結果の出力先
 * @return 0 on success, -1 on error (errno set)
 */
int eval_sum(const char *expression, int *out);

/**
 * @brief クエリ文字列から "query=" パラメータの値を抽出する
 *
 * @param query_string クエリ文字列（例: "query=1+2"）
 * @param out 抽出された値を格納するバッファへのポインタ（呼び出し側で解放が必要）
 * @return 0 on success, -1 on error (errno set)
 */
int extract_query_value_dup(const char *query_string, char **out);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CALC_H */
