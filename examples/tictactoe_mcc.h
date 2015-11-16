#ifndef __TICTACTOE_MCC_H__
#define __TICTACTOE_MCC_H__

/*
 * Public interface for the TicTacToe MUI custom class.
 * This class will be loaded automatically by Zune when the first
 * TicTacToe object will be created.
 * TicTacToe class from the GTK Tutorial by Ian Main and others,
 * MCC code from Tron.mcc by Klaus 'kmel' Melchior.
 */

#include <zune/zune.h>

/*** MUI Defines ***/

#define MUIC_TicTacToe "TicTacToe_mcc"
#define TicTacToeObject MUI_NewObject(MUIC_TicTacToe

/*** Methods ***/

enum {
    MUIM_TicTacToe_Clear = (TAG_USER | 0x77c80000 | 1),
};

/*** Method structs ***/

struct MUIP_TicTacToe_Clear {
    ULONG MethodID;
};

/*** Special method values ***/

/*** Special method flags ***/

/*** Attributes ***/

enum {
    MUIA_TicTacToe_Done = (TAG_USER | 0x77c80000 | 1),
};

/*** Special attribute values ***/

/*** Structures, Flags & Values ***/

#endif /* __TICTACTOE_MCC_H__ */
