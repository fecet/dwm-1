#include <X11/XF86keysym.h>

static int showsystray                   = 1;         /* 是否显示托盘栏 */
static const int newclientathead         = 0;         /* 定义新窗口在栈顶还是栈底 */
static const unsigned int borderpx       = 2;         /* 窗口边框大小 */
static const unsigned int systraypinning = 1;         /* 托盘跟随的显示器 0代表不指定显示器 */
static const unsigned int systrayspacing = 1;         /* 托盘间距 */
static const unsigned int systrayspadding = 5;        /* 托盘和状态栏的间隙 */
static int gappi                         = 12;        /* 窗口与窗口 缝隙大小 */
static int gappo                         = 12;        /* 窗口与边缘 缝隙大小 */
static const int _gappo                  = 12;        /* 窗口与窗口 缝隙大小 不可变 用于恢复时的默认值 */
static const int _gappi                  = 12;        /* 窗口与边缘 缝隙大小 不可变 用于恢复时的默认值 */
static const int vertpad                 = 5;         /* vertical padding of bar */
static const int sidepad                 = 5;         /* horizontal padding of bar */
static const int overviewgappi           = 24;        /* overview时 窗口与边缘 缝隙大小 */
static const int overviewgappo           = 60;        /* overview时 窗口与窗口 缝隙大小 */
static const int showbar                 = 1;         /* 是否显示状态栏 */
static const int topbar                  = 1;         /* 指定状态栏位置 0底部 1顶部 */
static const float mfact                 = 0.6;       /* 主工作区 大小比例 */
static const int   nmaster               = 1;         /* 主工作区 窗口数量 */
static const unsigned int snap           = 10;        /* 边缘依附宽度 */
static const unsigned int baralpha       = 0xc0;      /* 状态栏透明度 */
static const unsigned int borderalpha    = 0xdd;      /* 边框透明度 */
static const char *fonts[]               = { "JetBrainsMono Nerd Font:style=medium:size=13", "monospace:size=13" };
static const char *colors[][3]           = {          /* 颜色设置 ColFg, ColBg, ColBorder */ 
    [SchemeNorm] = { "#bbbbbb", "#333333", "#444444" },
    [SchemeSel] = { "#ffffff", "#37474F", "#42A5F5" },
    [SchemeSelGlobal] = { "#ffffff", "#37474F", "#FFC0CB" },
    [SchemeHid] = { "#dddddd", NULL, NULL },
    [SchemeSystray] = { NULL, "#7799AA", NULL },
    [SchemeUnderline] = { "#7799AA", NULL, NULL }, 
    [SchemeNormTag] = { "#bbbbbb", "#333333", NULL },
    [SchemeSelTag] = { "#eeeeee", "#333333", NULL },
    [SchemeBarEmpty] = { NULL, "#111111", NULL },
};
static const unsigned int alphas[][3]    = {          /* 透明度设置 ColFg, ColBg, ColBorder */ 
    [SchemeNorm] = { OPAQUE, baralpha, borderalpha }, 
    [SchemeSel] = { OPAQUE, baralpha, borderalpha },
    [SchemeSelGlobal] = { OPAQUE, baralpha, borderalpha },
    [SchemeNormTag] = { OPAQUE, baralpha, borderalpha }, 
    [SchemeSelTag] = { OPAQUE, baralpha, borderalpha },
    [SchemeBarEmpty] = { NULL, 0xa0a, NULL },
    [SchemeStatusText] = { OPAQUE, 0x88, NULL },
};

/* 自定义脚本位置 */
static const char *autostartscript = "~/scripts/autostart.sh";
static const char *statusbarscript = "$DWM/statusbar/statusbar.sh";

/* 自定义 scratchpad instance */
static const char scratchpadname[] = "scratchpad";

/* 自定义tag名称 */
/* 自定义特定实例的显示状态 */
//            ﮸  ﭮ 切
// 对应的tag序号以及快捷键:   0:1  1:2  2:3  3:4  4:5  5:9  6:c  7:m  8:0  9:w 10:l
static const char *tags[] = { "", "", "", "", "", "", "", "", "ﬄ", "﬐", "" };
static const Rule rules[] = {
    /* class                 instance              title             tags mask     isfloating  isglobal    isnoborder monitor */
    {"obs",                  NULL,                 NULL,             1 << 5,       0,          0,          0,        -1 },
    /* {"chrome",               NULL,                 NULL,             1 << 6,       0,          0,          0,        -1 }, */
    /* {"Chromium",             NULL,                 NULL,             1 << 6,       0,          0,          0,        -1 }, */
    /* {"music",                NULL,                 NULL,             1 << 7,       1,          0,          1,        -1 }, */
    { NULL,                 "qq",                  NULL,             1 << 8,       0,          0,          1,        -1 },
    /* { NULL,                 "wechat.exe",          NULL,             1 << 9,       0,          0,          0,        -1 }, */
    /* { NULL,                 "wxwork.exe",          NULL,             1 << 10,      0,          0,          0,        -1 }, */
    { NULL,                  NULL,                "broken",          0,            1,          0,          0,        -1 },
    { "图片查看",           "图片查看",           "图片查看",        0,            1,          0,          0,        -1 },
    { "图片预览",           "图片预览",           "图片预览",        0,            1,          0,          0,        -1 },
    { NULL,                  NULL,                "crx_",            0,            1,          0,          0,        -1 },
    {"flameshot",            NULL,                 NULL,             0,            1,          0,          0,        -1 },
    {"wemeetapp",            NULL,                 NULL,             TAGMASK,      1,          1,          0,        -1 }, // 腾讯会议在切换tag时有诡异bug导致退出 变成global来规避该问题
    {"float",                NULL,                 NULL,             0,            1,          0,          0,        -1 }, // 特殊class client默认浮动
    {"noborder",             NULL,                 NULL,             0,            1,          0,          1,        -1 }, // 特殊class client默认无边框
    {"global",               NULL,                 NULL,             TAGMASK,      1,          1,          0,        -1 }, // 特殊class client全局于所有tag
};
static const char *overviewtag = "OVERVIEW";
static const Layout overviewlayout = { "舘",  overview };

/* 自定义布局 */
static const Layout layouts[] = {
    {"﬿", tile},      /* 主次栈 */
    {"﩯", magicgrid}, /* 网格 */
};

#define SHCMD(cmd)                                                             \
  {                                                                            \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                       \
  }
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG, cmd) \
    { MODKEY,              KEY, view,       {.ui = 1 << TAG, .v = cmd} }, \
    { MODKEY|ShiftMask,    KEY, tag,        {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask,  KEY, toggleview, {.ui = 1 << TAG} }, \

static const char *termcmd[] = {"alacritty", NULL};
static const char *browsercmd[] = {"microsoft-edge-dev",
                                   /* "--proxy-server=127.0.0.1:7890", */
                                   /* "--force-device-scale-factor=1.7",  */
                                   NULL};
static const char *screenshotcmd[] = {"xfce4-screenshooter", NULL};
static const char *notifycenter[] = {
    "kill", "-s", "USR1", "$(pidof", "deadd-notification-center)", NULL};

/* static const char *music[] = {"/opt/YesPlayMusic/yesplaymusic", NULL}; */
/* static const char *music[] = {"/opt/Listen1/listen1", "%U", NULL}; */
static const char *music[] = {
    "google-chrome-stable",
    "chrome-extension://npjapjnfoejopmnlljoaociichngopgf/listen1.html",
    "--new-window", NULL};

static const char *editorcmd[] = {"zsh", "--login",
                                  "/home/rok/scripts/start_neovide.sh", NULL};

static Key keys[] = {
    /* modifier            key              function          argument */
    {0, XF86XK_AudioLowerVolume, spawn, SHCMD("~/scripts/vol-down.sh")},
    {0, XF86XK_AudioMute, spawn, SHCMD("~/scripts/vol-toggle.sh")},
    {0, XF86XK_AudioRaiseVolume, spawn, SHCMD("~/scripts/vol-up.sh")},
    {MODKEY,
     XK_equal,
     togglesystray,
     {0}}, /* super +            |  切换 托盘栏显示状态 */
    {MODKEY,
     XK_j,
     focusstack,
     {.i = +1}}, /* super tab          |  本tag内切换聚焦窗口 */
    {MODKEY,
     XK_k,
     focusstack,
     {.i = -1}}, /* super up           |  本tag内切换聚焦窗口 */
    /* { MODKEY,              XK_Down,         focusstack,       {.i = +1} }, */
    /* super down         |  本tag内切换聚焦窗口 */

    {MODKEY,
     XK_Left,
     viewtoleft,
     {0}}, /* super left         |  聚焦到左边的tag */
    {MODKEY,
     XK_Right,
     viewtoright,
     {0}}, /* super right        |  聚焦到右边的tag */
    {MODKEY | ShiftMask,
     XK_Left,
     tagtoleft,
     {0}}, /* super shift left   |  将本窗口移动到左边tag */
    {MODKEY | ShiftMask,
     XK_Right,
     tagtoright,
     {0}}, /* super shift right  |  将本窗口移动到右边tag */

    {MODKEY,
     XK_a,
     toggleoverview,
     {0}}, /* super a            |  显示所有tag 或 跳转到聚焦窗口的tag */

    /* { MODKEY,              XK_comma,        setmfact,         {.f = -0.05} },
     */

    /* super ,            |  缩小主工作区 */
    /* { MODKEY,              XK_period,       setmfact,         {.f = +0.05} },
     */
    /* super .            |  放大主工作区 */
    {MODKEY, XK_h, setmfact, {.f = -0.05}},
    {MODKEY, XK_l, setmfact, {.f = +0.05}},

    {MODKEY, XK_r, hidewin, {0}}, /* super h            |  隐藏 窗口 */
    {MODKEY | ShiftMask,
     XK_r,
     restorewin,
     {0}}, /* super shift h      |  取消隐藏 窗口 */

    {MODKEY | ShiftMask,
     XK_Return,
     zoom,
     {0}}, /* super shift enter  |  将当前聚焦窗口置为主窗口 */

    {MODKEY,
     XK_t,
     togglefloating,
     {0}}, /* super t            |  开启/关闭 聚焦目标的float模式 */
    {MODKEY | ShiftMask,
     XK_t,
     toggleallfloating,
     {0}}, /* super shift t      |  开启/关闭 全部目标的float模式 */
    {MODKEY, XK_f, fullscreen, {0}}, /* super f            |  开启/关闭 全屏 */
    {MODKEY | ShiftMask,
     XK_f,
     togglebar,
     {0}}, /* super shift f      |  开启/关闭 状态栏 */
    /* { MODKEY,              XK_e,            incnmaster,       {.i = +1} }, */
    /* super e            |  改变主工作区窗口数量 (1 2中切换) */

    {MODKEY | ShiftMask, XK_d, incnmaster, {.i = +1}},
    {MODKEY | ShiftMask, XK_i, incnmaster, {.i = -1}},

    {MODKEY, XK_bracketleft, focusmon, {.i = -1}},
    {MODKEY, XK_bracketright, focusmon, {.i = +1}},
    {MODKEY | ShiftMask, XK_bracketleft, tagmon, {.i = -1}},
    {MODKEY | ShiftMask, XK_bracketright, tagmon, {.i = +1}},
    /* { MODKEY,              XK_b,            focusmon,         {.i = +1} }, */
    /* super b            |  光标移动到另一个显示器 */
    /* { MODKEY|ShiftMask,    XK_b,            tagmon,           {.i = +1} }, */

    /* super shift b      |  将聚焦窗口移动到另一个显示器 */

    /* { MODKEY,              XK_q,            killclient,       {0} }, */

    /* super q            |  关闭窗口 */
    {MODKEY | ShiftMask, XK_q, killclient, {0}},
    {MODKEY | ControlMask,
     XK_F12,
     quit,
     {0}}, /* super ctrl f12     |  退出dwm */

    {MODKEY | ShiftMask,
     XK_space,
     selectlayout,
     {.v = &layouts[1]}}, /* super shift space  |  切换到网格布局 */
    {MODKEY,
     XK_o,
     showonlyorall,
     {0}}, /* super o            |  切换 只显示一个窗口 / 全部显示 */

    {MODKEY | ControlMask,
     XK_equal,
     setgap,
     {.i = -6}}, /* super ctrl up      |  窗口增大 */
    {MODKEY | ControlMask,
     XK_minus,
     setgap,
     {.i = +6}}, /* super ctrl down    |  窗口减小 */
    {MODKEY | ControlMask,
     XK_space,
     setgap,
     {.i = 0}}, /* super ctrl space   |  窗口重置 */

    {MODKEY | ControlMask,
     XK_Up,
     movewin,
     {.ui = UP}}, /* super ctrl up      |  移动窗口 */
    {MODKEY | ControlMask,
     XK_Down,
     movewin,
     {.ui = DOWN}}, /* super ctrl down    |  移动窗口 */
    {MODKEY | ControlMask,
     XK_Left,
     movewin,
     {.ui = LEFT}}, /* super ctrl left    |  移动窗口 */
    {MODKEY | ControlMask,
     XK_Right,
     movewin,
     {.ui = RIGHT}}, /* super ctrl right   |  移动窗口 */

    {MODKEY | Mod1Mask,
     XK_Up,
     resizewin,
     {.ui = V_REDUCE}}, /* super ctrl up      |  调整窗口 */
    {MODKEY | Mod1Mask,
     XK_Down,
     resizewin,
     {.ui = V_EXPAND}}, /* super ctrl down    |  调整窗口 */
    {MODKEY | Mod1Mask,
     XK_Left,
     resizewin,
     {.ui = H_REDUCE}}, /* super ctrl left    |  调整窗口 */
    {MODKEY | Mod1Mask,
     XK_Right,
     resizewin,
     {.ui = H_EXPAND}}, /* super ctrl right   |  调整窗口 */

    /* spawn + SHCMD 执行对应命令 */
    /* { MODKEY|ShiftMask,    XK_q,            spawn,
       SHCMD("~/scripts/app-starter.sh killw") }, */
    /* {MODKEY, XK_apostrophe, spawn, SHCMD("~/scripts/floatterm.sh")}, */
    {MODKEY, XK_apostrophe, togglescratch, SHCMD("~/scripts/floatterm.sh")},
    /* { MODKEY,              XK_Return,       spawn,
       SHCMD("~/scripts/app-starter.sh st") }, */
    {MODKEY, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_c, spawn, {.v = browsercmd}},
    {MODKEY, XK_n, spawn, SHCMD("~/scripts/notify-center.sh")},
    /* {MODKEY, XK_t, spawn, SHCMD("~/scripts/trans.sh")}, */
    {MODKEY, XK_m, spawn, {.v = music}},
    {MODKEY, XK_i, spawn, {.v = editorcmd}},
    {0, XK_Print, spawn, SHCMD("flameshot gui -c -p ~/Pictures/screenshots")},
    /* { MODKEY|ShiftMask,    XK_a,            spawn,
       SHCMD("~/scripts/app-starter.sh flameshot") }, */
    /* { MODKEY,              XK_d,            spawn,
       SHCMD("~/scripts/app-starter.sh rofi") }, */
    /* { MODKEY,              XK_space,        spawn,
       SHCMD("~/scripts/app-starter.sh rofi_window") }, */
    /* { MODKEY,              XK_p,            spawn,
       SHCMD("~/scripts/app-starter.sh rofi_p") }, */
    /* { MODKEY|ShiftMask,    XK_k,            spawn,
       SHCMD("~/scripts/app-starter.sh screenkey") }, */
    /* { MODKEY,              XK_k,            spawn,
       SHCMD("~/scripts/app-starter.sh blurlock") }, */
    /* { MODKEY,              XK_F1,           spawn,
       SHCMD("~/scripts/app-starter.sh filemanager") }, */
    /* { MODKEY|ShiftMask,    XK_Up,           spawn,
       SHCMD("~/scripts/app-starter.sh set_vol up &") }, */
    /* { MODKEY|ShiftMask,    XK_Down,         spawn,
       SHCMD("~/scripts/app-starter.sh set_vol down &") }, */
    /* { MODKEY,              XK_j,            spawn,
       SHCMD("~/scripts/app-starter.sh robot") }, */
    /* { MODKEY|ShiftMask,    XK_j,            spawn,
       SHCMD("~/scripts/app-starter.sh robot onlyclick") }, */
    /* { ShiftMask|ControlMask, XK_c,          spawn,            SHCMD("xclip -o
       | xclip -selection c") }, */

    /* super key : 跳转到对应tag (可附加一条命令 若目标目录无窗口，则执行该命令) */
    /* super shift key : 将聚焦窗口移动到对应tag */
    /* key tag cmd */
    TAGKEYS(XK_1, 0,  0)
    TAGKEYS(XK_2, 1,  0)
    TAGKEYS(XK_3, 2,  0)
    TAGKEYS(XK_4, 3,  0)
    TAGKEYS(XK_5, 4,  0)
    TAGKEYS(XK_6, 5,  0)
    /* TAGKEYS(XK_9, 5,  "obs") */
    /* TAGKEYS(XK_c, 6,  "google-chrome-stable") */
    /* TAGKEYS(XK_m, 7,  "~/scripts/music_player.sh") */
    TAGKEYS(XK_0, 8,  "linuxqq")
    /* TAGKEYS(XK_w, 9,  "/opt/apps/com.qq.weixin.deepin/files/run.sh") */
    /* TAGKEYS(XK_l, 10, "/opt/apps/com.qq.weixin.work.deepin/files/run.sh") */
};
static Button buttons[] = {
    /* click               event mask       button            function       argument  */
    /* 点击窗口标题栏操作 */
    { ClkWinTitle,         0,               Button1,          hideotherwins, {0} },                                   // 左键        |  点击标题     |  隐藏其他窗口仅保留该窗口
    { ClkWinTitle,         0,               Button3,          togglewin,     {0} },                                   // 右键        |  点击标题     |  切换窗口显示状态
    /* 点击窗口操作 */
    { ClkClientWin,        MODKEY,          Button1,          movemouse,     {0} },                                   // super+左键  |  拖拽窗口     |  拖拽窗口
    { ClkClientWin,        MODKEY,          Button3,          resizemouse,   {0} },                                   // super+右键  |  拖拽窗口     |  改变窗口大小
    /* 点击tag操作 */
    { ClkTagBar,           0,               Button1,          view,          {0} },                                   // 左键        |  点击tag      |  切换tag
	{ ClkTagBar,           0,               Button3,          toggleview,    {0} },                                   // 右键        |  点击tag      |  切换是否显示tag
    { ClkTagBar,           MODKEY,          Button1,          tag,           {0} },                                   // super+左键  |  点击tag      |  将窗口移动到对应tag
    { ClkTagBar,           0,               Button4,          viewtoleft,    {0} },                                   // 鼠标滚轮上  |  tag          |  向前切换tag
	{ ClkTagBar,           0,               Button5,          viewtoright,   {0} },                                   // 鼠标滚轮下  |  tag          |  向后切换tag
    /* 点击状态栏操作 */
    { ClkStatusText,       0,               Button1,          clickstatusbar,{0} },                                   // 左键        |  点击状态栏   |  根据状态栏的信号执行 ~/scripts/dwmstatusbar.sh $signal L
    { ClkStatusText,       0,               Button2,          clickstatusbar,{0} },                                   // 中键        |  点击状态栏   |  根据状态栏的信号执行 ~/scripts/dwmstatusbar.sh $signal M
    { ClkStatusText,       0,               Button3,          clickstatusbar,{0} },                                   // 右键        |  点击状态栏   |  根据状态栏的信号执行 ~/scripts/dwmstatusbar.sh $signal R
    { ClkStatusText,       0,               Button4,          clickstatusbar,{0} },                                   // 鼠标滚轮上  |  状态栏       |  根据状态栏的信号执行 ~/scripts/dwmstatusbar.sh $signal U
    { ClkStatusText,       0,               Button5,          clickstatusbar,{0} },                                   // 鼠标滚轮下  |  状态栏       |  根据状态栏的信号执行 ~/scripts/dwmstatusbar.sh $signal D
                                                                                                                      //
    /* 点击bar空白处 */
    { ClkBarEmpty,         0,               Button1,          spawn, SHCMD("~/scripts/call_rofi.sh window") },        // 左键        |  bar空白处    |  rofi 执行 window
    { ClkBarEmpty,         0,               Button3,          spawn, SHCMD("~/scripts/call_rofi.sh drun") },          // 右键        |  bar空白处    |  rofi 执行 drun
};
