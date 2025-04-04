//Grup 28 av Theophilous och Samuel Memory game
#include "28.h"

static nrfx_uarte_t uarte_instance = NRFX_UARTE_INSTANCE(UART_INSTANCE_ID);

bool running = true;
uint32_t game_round = 0; // used to pseudo-randomize computer move

void uart_send(char *msg) {
    nrfx_uarte_tx(&uarte_instance, (uint8_t *)msg, strlen(msg), 0);
    while (nrfx_uarte_tx_in_progress(&uarte_instance));
}


void clear_screen(void) {
    uint8_t msg3 [] = "\033c";
    uart_send(msg3);
}

char* send_string(char move) {
    switch(move) {
        case 'r': return "Rock";
        case 'p': return "Paper";
        case 's': return "Scissors";
        default:  return "Unknown";
    }
}

// Process one round of rock paper scissors.
void process_round(char user_move) {
    // Pseudo-random computer move based on game_round (cycling through r, p, s).
    char moves[3] = {'r', 'p', 's'};
    char computer_move = moves[game_round % 3];
    game_round++;

    char result[32];
    if (user_move == computer_move) {
        strcpy(result, "Tie!");
    } else {
        if (user_move == 'r') {
            if (computer_move == 's') {
                strcpy(result, "You win!");
            } else {
                strcpy(result, "You lose!");
            }
        } else if (user_move == 'p') {
            if (computer_move == 'r') {
                strcpy(result, "You win!");
            } else {
                strcpy(result, "You lose!");
            }
        } else if (user_move == 's') {
            if (computer_move == 'p') {
                strcpy(result, "You win!");
            } else {
                strcpy(result, "You lose!");
            }
        } else {
            strcpy(result, "Invalid move");
        }
    }
    
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "\r\nYou chose: %s\r\nComputer chose: %s\r\nResult: %s\r\n\r\n",
    send_string(user_move), send_string(computer_move), result);
    uart_send(buffer);
}

// UARTE event handler processes incoming characters.
void uarte_event_handler(nrfx_uarte_event_t const *p_event, void *p_context) {
    if (p_event->type == NRFX_UARTE_EVT_RX_DONE) {
        char c = (char)p_event->data.rx.p_data[0];
        switch (c) {
            case 'r':
            case 'p':
            case 's':
                process_round(c);
                break;
            case 'q':
            {
                 uint8_t exitMsg[] = "\r\nExiting game.\r\n";
                uart_send(exitMsg);
                running = false;
            }
                break;
            default:
            {
                uint8_t msg3[] = "\r\nInvalid input. Please use [r], [p], [s] for a move, or [q] to quit.\r\n";
                uart_send(msg3);
            }
                break;
        }
        nrfx_uarte_rx(&uarte_instance, p_event->data.rx.p_data, 1);
    }
}

// Main game function to initialize UARTE and start the game loop.
int start_game_grupp28(void) {
    nrfx_err_t err;
    nrfx_uarte_config_t uarte_config = NRFX_UARTE_DEFAULT_CONFIG(UART_TX_PIN, UART_RX_PIN);
    err = nrfx_uarte_init(&uarte_instance, &uarte_config, uarte_event_handler);
    assert(err == NRFX_SUCCESS);

    nrfx_systick_init();

    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(UART_INSTANCE_ID)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(UART_INSTANCE_ID)));

    static uint8_t rx_buffer;
    err = nrfx_uarte_rx(&uarte_instance, &rx_buffer, 1);
    assert(err == NRFX_SUCCESS);

    clear_screen();

    uint8_t msg1[] = "Rock-Paper-Scissors Game\r\n";
    uart_send(msg1);
    static uint8_t msg2[] = "Enter your move: [r]ock, [p]aper, [s]cissors, or [q]uit:\r\n";
    uart_send(msg2);

    while (running) {
        nrfx_systick_delay_ms(100);
    }

    nrfx_uarte_uninit(&uarte_instance);
    return 0;
}