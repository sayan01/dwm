/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx  = 4;        /* border pixel of windows */
static unsigned int gappx     = 8;        /* gaps between windows */
static unsigned int snap      = 32;       /* snap pixel */
static int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static unsigned int systrayspacing = 4;   /* systray spacing */
static int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static int showsystray        = 1;     /* 0 means no systray */
static int showbar            = 1;     /* 0 means no bar */
static int topbar             = 1;     /* 0 means bottom bar */
static int user_bh            = 28;    /* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
static const char *fonts[]          = {
	"monospace:size=10",
	"fontawesome:size=10",
	"Noto Color Emoji:size=10",
};
static char dmenufont[]       = "Bitstream Vera Sans:size=12";
// background color
static char normbgcolor[]       = "#000000";
// inactive window border color
static char normbordercolor[]       = "#dddddd";
// font color
static char normfgcolor[]       = "#bbbbbb";
// current tag and current window font color (font on cyan)
static char selfgcolor[]       = "#000000";
// top bar second color (blue) and active window border color (cyan)
static char selbordercolor[]        = "#21c353";
static char selbgcolor[]        = "#21c353";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};
/* tagging */
static const char *tags[] = { "", "", "", "", "", "6", "7", "8", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	//                               1 << 3 means 1000 which means tag = 4
	/* class                 instance  title tags isfloat isterm noswallow monitor*/
	//							    |      |    |        |     |   |       /
	// floating windows				|      |    |        |     |   |      /
	// 								V      V     \v      V     V   V     V
	{ "Gimp",                      NULL,  NULL,    0,     1,   0,  0,  -1 },
	{ "mpv",                       NULL,  NULL,    0,     1,   0,  0,  -1 },
	{ "ksnip",                     NULL,  NULL,    0,     1,   0,  0,  -1 },
	{ "kdeconnect.daemon",         NULL,  NULL,    0,     1,   0,  0,  -1 },

	// fixed desktop
	{ "TelegramDesktop",           NULL,  NULL, 1 << 1,   0,   0,  0,  -1 },
	{ "discord",                   NULL,  NULL, 1 << 1,   0,   0,  0,  -1 },
    { "whatsapp-nativefier-d52542",NULL,  NULL, 1 << 1,   0,   0,  0,  -1 },
    { "whatsapp-nativefier"       ,NULL,  NULL, 1 << 1,   0,   0,  0,  -1 },
    { "Whatsapp-for-linux"       ,NULL,  NULL, 1 << 1,   0,   0,  0,  -1 },
    { "obs",                       NULL,  NULL, 1 << 4,   0,   0,  0,  -1 },
    { "zoom",                      NULL,  NULL, 1 << 3,   0,   0,  0,  -1 },
    { "Spotify",                   NULL,  NULL, 1 << 2,   0,   0,  0,  -1 },
    {  NULL,                  "spotify",  NULL, 1 << 2,   0,   0,  0,  -1 },
    { "spot",                      NULL,  NULL, 1 << 2,   0,   0,  0,  -1 },
    {  NULL,                     "spot",  NULL, 1 << 2,   0,   0,  0,  -1 },
    {  "Thunderbird",              NULL,  NULL, 1 << 8,   0,   0,  0,  -1 },

	// isterm 
	{ "konsole",                   NULL,  NULL,    0,     0,   1,  0,  -1 },
	{ "st",                        NULL,  NULL,    0,     0,   1,  0,  -1 },
	// no swallow
	{ NULL,              NULL,  "Event Tester",    0,     0,   0,  1,  -1 },
};

/* layout(s) */
static float mfact = 0.5; // factor of master area size [0.05..0.95] 
static int nmaster = 1;    /* number of clients in master area */
static int resizehints = 0;// 1 = respect size hints in tiled resizals

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "T",           tile },  /* first entry is default */
	{ "F",           NULL },  /* no layout function means floating behavior */
	{ "M",           monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/bash", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "konsole", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
	{ "normbgcolor",        STRING,  &normbgcolor },
	{ "normbordercolor",    STRING,  &normbordercolor },
	{ "normfgcolor",        STRING,  &normfgcolor },
	{ "selbgcolor",         STRING,  &selbgcolor },
	{ "selbordercolor",     STRING,  &selbordercolor },
	{ "selfgcolor",         STRING,  &selfgcolor },
	{ "borderpx",          	INTEGER, &borderpx },
	{ "snap",          		INTEGER, &snap },
	{ "showbar",          	INTEGER, &showbar },
	{ "topbar",          	INTEGER, &topbar },
	{ "nmaster",          	INTEGER, &nmaster },
	{ "resizehints",       	INTEGER, &resizehints },
	{ "mfact",      	 	FLOAT,   &mfact },
	{ "gappx",				INTEGER, &gappx },
	{ "swallowfloating",	INTEGER, &swallowfloating },
	{ "systraypinning",		INTEGER, &systraypinning},
	{ "systrayspacing",		INTEGER, &systrayspacing },
{ "systraypinningfailfirst",INTEGER, &systraypinningfailfirst },
	{ "showsystray",		INTEGER, &showsystray },
	{ "user_bh",			INTEGER, &user_bh },
};



static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_F1,     togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ Mod1Mask,                     XK_Tab,    swapfocus,  	   {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|Mod1Mask,              XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ ControlMask|ShiftMask,        XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ 0,                            XK_F11,      togglefullscr,  {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ Mod1Mask,                     XK_minus,  setgaps,        {.i = -1 } },
	{ Mod1Mask,                     XK_equal,  setgaps,        {.i = +1 } },
	{ Mod1Mask|ShiftMask,           XK_equal,  setgaps,        {.i = 0  } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
