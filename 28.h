//Grup 28 av Theophilous och Samuel Memory game
#ifndef H_28_H
#define H_28_H

#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrfx_uarte.h>
#include <nrfx_systick.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define UART_INSTANCE_ID 0
#define UART_TX_PIN 20
#define UART_RX_PIN 22

extern bool running;
extern uint32_t game_round;

void uart_send(char *msg);
void clear_screen(void);
char* send_string(char move);
void process_round(char user_move);
void uarte_event_handler(nrfx_uarte_event_t const *p_event, void *p_context);
int start_game_grupp28(void);

#endif