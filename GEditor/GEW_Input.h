#ifndef GEW_INPUT_H
#define GEW_INPUT_H

#define GUK_ESCAPE          0x01
#define GUK_1               0x02
#define GUK_2               0x03
#define GUK_3               0x04
#define GUK_4               0x05
#define GUK_5               0x06
#define GUK_6               0x07
#define GUK_7               0x08
#define GUK_8               0x09
#define GUK_9               0x0A
#define GUK_0               0x0B
#define GUK_MINUS           0x0C    /* - on main keyboard */
#define GUK_EQUALS          0x0D
#define GUK_BACK            0x0E    /* backspace */
#define GUK_TAB             0x0F
#define GUK_Q               0x10
#define GUK_W               0x11
#define GUK_E               0x12
#define GUK_R               0x13
#define GUK_T               0x14
#define GUK_Y               0x15
#define GUK_U               0x16
#define GUK_I               0x17
#define GUK_O               0x18
#define GUK_P               0x19
#define GUK_LBRACKET        0x1A
#define GUK_RBRACKET        0x1B
#define GUK_RETURN          0x1C    /* Enter on main keyboard */
#define GUK_LCONTROL        0x1D
#define GUK_A               0x1E
#define GUK_S               0x1F
#define GUK_D               0x20
#define GUK_F               0x21
#define GUK_G               0x22
#define GUK_H               0x23
#define GUK_J               0x24
#define GUK_K               0x25
#define GUK_L               0x26
#define GUK_SEMICOLON       0x27
#define GUK_APOSTROPHE      0x28
#define GUK_GRAVE           0x29    /* accent grave */
#define GUK_LSHIFT          0x2A
#define GUK_BACKSLASH       0x2B
#define GUK_Z               0x2C
#define GUK_X               0x2D
#define GUK_C               0x2E
#define GUK_V               0x2F
#define GUK_B               0x30
#define GUK_N               0x31
#define GUK_M               0x32
#define GUK_COMMA           0x33
#define GUK_PERIOD          0x34    /* . on main keyboard */
#define GUK_SLASH           0x35    /* / on main keyboard */
#define GUK_RSHIFT          0x36
#define GUK_MULTIPLY        0x37    /* * on numeric keypad */
#define GUK_LMENU           0x38    /* left Alt */
#define GUK_SPACE           0x39
#define GUK_CAPITAL         0x3A
#define GUK_F1              0x3B
#define GUK_F2              0x3C
#define GUK_F3              0x3D
#define GUK_F4              0x3E
#define GUK_F5              0x3F
#define GUK_F6              0x40
#define GUK_F7              0x41
#define GUK_F8              0x42
#define GUK_F9              0x43
#define GUK_F10             0x44
#define GUK_NUMLOCK         0x45
#define GUK_SCROLL          0x46    /* Scroll Lock */
#define GUK_NUMPAD7         0x47
#define GUK_NUMPAD8         0x48
#define GUK_NUMPAD9         0x49
#define GUK_SUBTRACT        0x4A    /* - on numeric keypad */
#define GUK_NUMPAD4         0x4B
#define GUK_NUMPAD5         0x4C
#define GUK_NUMPAD6         0x4D
#define GUK_ADD             0x4E    /* + on numeric keypad */
#define GUK_NUMPAD1         0x4F
#define GUK_NUMPAD2         0x50
#define GUK_NUMPAD3         0x51
#define GUK_NUMPAD0         0x52
#define GUK_DECIMAL         0x53    /* . on numeric keypad */
#define GUK_OEM_102         0x56    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define GUK_F11             0x57
#define GUK_F12             0x58
#define GUK_F13             0x64    /*                     (NEC PC98) */
#define GUK_F14             0x65    /*                     (NEC PC98) */
#define GUK_F15             0x66    /*                     (NEC PC98) */
#define GUK_KANA            0x70    /* (Japanese keyboard)            */
#define GUK_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define GUK_CONVERT         0x79    /* (Japanese keyboard)            */
#define GUK_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define GUK_YEN             0x7D    /* (Japanese keyboard)            */
#define GUK_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define GUK_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define GUK_PREVTRACK       0x90    /* Previous Track (GUK_CIRCUMFLEX on Japanese keyboard) */
#define GUK_AT              0x91    /*                     (NEC PC98) */
#define GUK_COLON           0x92    /*                     (NEC PC98) */
#define GUK_UNDERLINE       0x93    /*                     (NEC PC98) */
#define GUK_KANJI           0x94    /* (Japanese keyboard)            */
#define GUK_STOP            0x95    /*                     (NEC PC98) */
#define GUK_AX              0x96    /*                     (Japan AX) */
#define GUK_UNLABELED       0x97    /*                        (J3100) */
#define GUK_NEXTTRACK       0x99    /* Next Track */
#define GUK_NUMPADENTER     0x9C    /* Enter on numeric keypad */
#define GUK_RCONTROL        0x9D
#define GUK_MUTE            0xA0    /* Mute */
#define GUK_CALCULATOR      0xA1    /* Calculator */
#define GUK_PLAYPAUSE       0xA2    /* Play / Pause */
#define GUK_MEDIASTOP       0xA4    /* Media Stop */
#define GUK_VOLUMEDOWN      0xAE    /* Volume - */
#define GUK_VOLUMEUP        0xB0    /* Volume + */
#define GUK_WEBHOME         0xB2    /* Web home */
#define GUK_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define GUK_DIVIDE          0xB5    /* / on numeric keypad */
#define GUK_SYSRQ           0xB7
#define GUK_RMENU           0xB8    /* right Alt */
#define GUK_PAUSE           0xC5    /* Pause */
#define GUK_HOME            0xC7    /* Home on arrow keypad */
#define GUK_UP              0xC8    /* UpArrow on arrow keypad */
#define GUK_PRIOR           0xC9    /* PgUp on arrow keypad */
#define GUK_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define GUK_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define GUK_END             0xCF    /* End on arrow keypad */
#define GUK_DOWN            0xD0    /* DownArrow on arrow keypad */
#define GUK_NEXT            0xD1    /* PgDn on arrow keypad */
#define GUK_INSERT          0xD2    /* Insert on arrow keypad */
#define GUK_DELETE          0xD3    /* Delete on arrow keypad */
#define GUK_LWIN            0xDB    /* Left Windows key */
#define GUK_RWIN            0xDC    /* Right Windows key */
#define GUK_APPS            0xDD    /* AppMenu key */
#define GUK_POWER           0xDE    /* System Power */
#define GUK_SLEEP           0xDF    /* System Sleep */
#define GUK_WAKE            0xE3    /* System Wake */
#define GUK_WEBSEARCH       0xE5    /* Web Search */
#define GUK_WEBFAVORITES    0xE6    /* Web Favorites */
#define GUK_WEBREFRESH      0xE7    /* Web Refresh */
#define GUK_WEBSTOP         0xE8    /* Web Stop */
#define GUK_WEBFORWARD      0xE9    /* Web Forward */
#define GUK_WEBBACK         0xEA    /* Web Back */
#define GUK_MYCOMPUTER      0xEB    /* My Computer */
#define GUK_MAIL            0xEC    /* Mail */
#define GUK_MEDIASELECT     0xED    /* Media Select */

/*
*  Alternate names for keys, to facilitate transition from DOS.
*/
#define GUK_BACKSPACE       GUK_BACK            /* backspace */
#define GUK_NUMPADSTAR      GUK_MULTIPLY        /* * on numeric keypad */
#define GUK_LALT            GUK_LMENU           /* left Alt */
#define GUK_CAPSLOCK        GUK_CAPITAL         /* CapsLock */
#define GUK_NUMPADMINUS     GUK_SUBTRACT        /* - on numeric keypad */
#define GUK_NUMPADPLUS      GUK_ADD             /* + on numeric keypad */
#define GUK_NUMPADPERIOD    GUK_DECIMAL         /* . on numeric keypad */
#define GUK_NUMPADSLASH     GUK_DIVIDE          /* / on numeric keypad */
#define GUK_RALT            GUK_RMENU           /* right Alt */
#define GUK_UPARROW         GUK_UP              /* UpArrow on arrow keypad */
#define GUK_PGUP            GUK_PRIOR           /* PgUp on arrow keypad */
#define GUK_LEFTARROW       GUK_LEFT            /* LeftArrow on arrow keypad */
#define GUK_RIGHTARROW      GUK_RIGHT           /* RightArrow on arrow keypad */
#define GUK_DOWNARROW       GUK_DOWN            /* DownArrow on arrow keypad */
#define GUK_PGDN            GUK_NEXT            /* PgDn on arrow keypad */

#define GUM_BUTTON_LEFT		0x01
#define GUM_BUTTON_RIGHT	0x02
#define GUM_BUTTON_MIDDLE	0x04
#define GUM_BUTTON_X1		0x08
#define GUM_BUTTON_X2		0x10

class GEW_Input
{
public:
	struct Data
	{
		bool	Keyboard[ 256 ];
		int		MButtons;
		int		X;
		int		Y;
		int		Dx;
		int		Dy;
		int		Dw;

		bool isKeyDown( int i_gUK );
		bool isMouseDown( int i_mouseButton );
		Data();
	};
};

#endif