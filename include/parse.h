#ifndef __PARSE_H__
#define __PARSE_H__



/**
 * @brief The function saves published message
 * @param[in] message reference to received message.
 *
 * @param[in] size size of the received message.
 *
 * @returns none
 */
 void parse_save(const char* message, size_t size);


/**
 * @brief returns first message in stack.
 *
 * @param[in] none
 *
 * @returns NULL if no mesages stored
 */
 char* parse_get_mess(void);
/**
 * @brief decriments counters, prepares next message to be sent
 *
 * @param[in] none
 *
 * @returns none
 */
 void parse_prepare_mess(void);

#endif

