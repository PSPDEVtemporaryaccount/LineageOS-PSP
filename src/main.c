#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspiofilemgr.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

PSP_MODULE_INFO("LineageOS PSP", 0, 2, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define SCREEN_W 480
#define SCREEN_H 272
#define FB_STRIDE 512
#define FB_SIZE (FB_STRIDE * SCREEN_H)
#define FB0 ((unsigned int *)0x44000000)
#define FB1 ((unsigned int *)(0x44000000 + FB_SIZE * 4))

typedef enum { SCREEN_HOME, SCREEN_SETTINGS, SCREEN_RECOVERY } Screen;

static LineageConfig g_config;
static Screen g_screen = SCREEN_HOME;
static int g_selection = 0;
static int g_running = 1;
static int g_frame = 0;
static unsigned int *g_back = FB0;
static unsigned int *g_front = FB1;
static char g_status[64] = "Bienvenue dans LineageOS PSP";

#define RGBA(r,g,b,a) ((unsigned int)(((a)<<24)|((b)<<16)|((g)<<8)|(r)))
#define C_BG       RGBA(15, 17, 27, 255)
#define C_PANEL    RGBA(26, 29, 44, 255)
#define C_PANEL2   RGBA(34, 38, 56, 255)
#define C_TEXT     RGBA(245, 244, 250, 255)
#define C_MUTED    RGBA(151, 154, 174, 255)
#define C_ACCENT   RGBA(184, 139, 255, 255)
#define C_ACCENT_D RGBA(83, 59, 125, 255)
#define C_GREEN    RGBA(111, 221, 177, 255)
#define C_ORANGE   RGBA(255, 194, 112, 255)
#define C_DARK_BG  RGBA(15, 17, 27, 255)
#define C_LIGHT_BG RGBA(231, 234, 243, 255)

static unsigned int color(unsigned int dark, unsigned int light)
{
    return g_config.dark_theme ? dark : light;
}

static void pixel(int x, int y, unsigned int c)
{
    if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H)
        g_back[y * FB_STRIDE + x] = c;
}

static void fill_rect(int x, int y, int w, int h, unsigned int c)
{
    int xx, yy;
    int x2 = x + w, y2 = y + h;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x2 > SCREEN_W) x2 = SCREEN_W;
    if (y2 > SCREEN_H) y2 = SCREEN_H;
    for (yy = y; yy < y2; ++yy)
        for (xx = x; xx < x2; ++xx)
            pixel(xx, yy, c);
}

static void outline_rect(int x, int y, int w, int h, unsigned int c)
{
    fill_rect(x, y, w, 2, c);
    fill_rect(x, y + h - 2, w, 2, c);
    fill_rect(x, y, 2, h, c);
    fill_rect(x + w - 2, y, 2, h, c);
}

static const unsigned char *glyph(char c)
{
    static const unsigned char blank[5] = {0,0,0,0,0};
    static const unsigned char A[5] = {0x7E,0x11,0x11,0x11,0x7E};
    static const unsigned char B[5] = {0x7F,0x49,0x49,0x49,0x36};
    static const unsigned char C[5] = {0x3E,0x41,0x41,0x41,0x22};
    static const unsigned char D[5] = {0x7F,0x41,0x41,0x22,0x1C};
    static const unsigned char E[5] = {0x7F,0x49,0x49,0x49,0x41};
    static const unsigned char F[5] = {0x7F,0x09,0x09,0x09,0x01};
    static const unsigned char G[5] = {0x3E,0x41,0x49,0x49,0x7A};
    static const unsigned char H[5] = {0x7F,0x08,0x08,0x08,0x7F};
    static const unsigned char I[5] = {0x00,0x41,0x7F,0x41,0x00};
    static const unsigned char J[5] = {0x20,0x40,0x41,0x3F,0x01};
    static const unsigned char K[5] = {0x7F,0x08,0x14,0x22,0x41};
    static const unsigned char L[5] = {0x7F,0x40,0x40,0x40,0x40};
    static const unsigned char M[5] = {0x7F,0x02,0x0C,0x02,0x7F};
    static const unsigned char N[5] = {0x7F,0x04,0x08,0x10,0x7F};
    static const unsigned char O[5] = {0x3E,0x41,0x41,0x41,0x3E};
    static const unsigned char P[5] = {0x7F,0x09,0x09,0x09,0x06};
    static const unsigned char Q[5] = {0x3E,0x41,0x51,0x21,0x5E};
    static const unsigned char R[5] = {0x7F,0x09,0x19,0x29,0x46};
    static const unsigned char S[5] = {0x46,0x49,0x49,0x49,0x31};
    static const unsigned char T[5] = {0x01,0x01,0x7F,0x01,0x01};
    static const unsigned char U[5] = {0x3F,0x40,0x40,0x40,0x3F};
    static const unsigned char V[5] = {0x1F,0x20,0x40,0x20,0x1F};
    static const unsigned char W[5] = {0x3F,0x40,0x38,0x40,0x3F};
    static const unsigned char X[5] = {0x63,0x14,0x08,0x14,0x63};
    static const unsigned char Y[5] = {0x07,0x08,0x70,0x08,0x07};
    static const unsigned char Z[5] = {0x61,0x51,0x49,0x45,0x43};
    static const unsigned char n0[5] = {0x3E,0x51,0x49,0x45,0x3E};
    static const unsigned char n1[5] = {0x00,0x42,0x7F,0x40,0x00};
    static const unsigned char n2[5] = {0x42,0x61,0x51,0x49,0x46};
    static const unsigned char n3[5] = {0x21,0x41,0x45,0x4B,0x31};
    static const unsigned char n4[5] = {0x18,0x14,0x12,0x7F,0x10};
    static const unsigned char n5[5] = {0x27,0x45,0x45,0x45,0x39};
    static const unsigned char n6[5] = {0x3C,0x4A,0x49,0x49,0x30};
    static const unsigned char n7[5] = {0x01,0x71,0x09,0x05,0x03};
    static const unsigned char n8[5] = {0x36,0x49,0x49,0x49,0x36};
    static const unsigned char n9[5] = {0x06,0x49,0x49,0x29,0x1E};
    static const unsigned char dash[5] = {0x08,0x08,0x08,0x08,0x08};
    static const unsigned char dot[5] = {0x00,0x60,0x60,0x00,0x00};
    static const unsigned char colon[5] = {0x00,0x36,0x36,0x00,0x00};
    static const unsigned char slash[5] = {0x20,0x10,0x08,0x04,0x02};
    static const unsigned char ex[5] = {0x00,0x00,0x5F,0x00,0x00};
    static const unsigned char plus[5] = {0x08,0x08,0x3E,0x08,0x08};
    static const unsigned char question[5] = {0x02,0x01,0x51,0x09,0x06};
    static const unsigned char apost[5] = {0x00,0x05,0x03,0x00,0x00};
    static const unsigned char percent[5] = {0x23,0x13,0x08,0x64,0x62};
    switch (c) {
        case 'A': return A; case 'B': return B; case 'C': return C;
        case 'D': return D; case 'E': return E; case 'F': return F;
        case 'G': return G; case 'H': return H; case 'I': return I;
        case 'J': return J; case 'K': return K; case 'L': return L;
        case 'M': return M; case 'N': return N; case 'O': return O;
        case 'P': return P; case 'Q': return Q; case 'R': return R;
        case 'S': return S; case 'T': return T; case 'U': return U;
        case 'V': return V; case 'W': return W; case 'X': return X;
        case 'Y': return Y; case 'Z': return Z;
        case '0': return n0; case '1': return n1; case '2': return n2;
        case '3': return n3; case '4': return n4; case '5': return n5;
        case '6': return n6; case '7': return n7; case '8': return n8;
        case '9': return n9; case '-': return dash; case '.': return dot;
        case ':': return colon; case '/': return slash; case '!': return ex;
        case '+': return plus; case '?': return question; case '\'': return apost;
        case '%': return percent; default: return blank;
    }
}

static void draw_char(int x, int y, char ch, unsigned int c, int scale)
{
    const unsigned char *bits = glyph((char)toupper((unsigned char)ch));
    int col, row, sx, sy;
    for (col = 0; col < 5; ++col) {
        for (row = 0; row < 7; ++row) {
            if (bits[col] & (1u << row)) {
                for (sy = 0; sy < scale; ++sy)
                    for (sx = 0; sx < scale; ++sx)
                        pixel(x + col * scale + sx, y + row * scale + sy, c);
            }
        }
    }
}

static void draw_text(int x, int y, const char *s, unsigned int c, int scale)
{
    while (*s) {
        if (*s == '\n') { y += 10 * scale; x = 0; }
        else draw_char(x, y, *s, c, scale);
        x += 6 * scale;
        ++s;
    }
}

static void draw_centered(int y, const char *s, unsigned int c, int scale)
{
    int width = (int)strlen(s) * 6 * scale;
    draw_text((SCREEN_W - width) / 2, y, s, c, scale);
}

static void draw_icon(int x, int y, char letter, unsigned int bg)
{
    fill_rect(x, y, 34, 34, bg);
    draw_char(x + 12, y + 13, letter, C_TEXT, 1);
}

static void draw_topbar(const char *section)
{
    fill_rect(0, 0, SCREEN_W, 31, color(C_PANEL, RGBA(250,251,255,255)));
    fill_rect(0, 29, SCREEN_W, 2, C_ACCENT);
    draw_text(14, 10, "LINEAGEOS PSP", color(C_TEXT, RGBA(35,35,50,255)), 1);
    draw_text(363, 10, "0.2.0", C_ACCENT, 1);
    draw_text(14, 42, section, color(C_MUTED, RGBA(95,98,116,255)), 1);
}

static void draw_bottom_hint(const char *hint)
{
    fill_rect(0, 249, SCREEN_W, 23, color(C_PANEL, RGBA(250,251,255,255)));
    fill_rect(0, 249, SCREEN_W, 1, color(C_PANEL2, RGBA(215,218,230,255)));
    draw_text(12, 258, hint, color(C_MUTED, RGBA(95,98,116,255)), 1);
}

static void draw_card(int x, int y, int w, int h, int selected,
                      char icon, const char *title, const char *subtitle,
                      unsigned int icon_bg)
{
    unsigned int base = color(C_PANEL, RGBA(250,251,255,255));
    fill_rect(x, y, w, h, base);
    if (selected) {
        outline_rect(x - 2, y - 2, w + 4, h + 4, C_ACCENT);
        fill_rect(x, y + h - 3, w, 3, C_ACCENT);
    } else {
        outline_rect(x, y, w, h, color(C_PANEL2, RGBA(220,223,235,255)));
    }
    draw_icon(x + 12, y + 13, icon, icon_bg);
    draw_text(x + 56, y + 17, title, color(C_TEXT, RGBA(35,35,50,255)), 1);
    draw_text(x + 56, y + 34, subtitle, color(C_MUTED, RGBA(105,108,126,255)), 1);
}

static void draw_home(void)
{
    unsigned int panel = color(C_PANEL, RGBA(250,251,255,255));
    fill_rect(0, 0, SCREEN_W, SCREEN_H, color(C_BG, C_LIGHT_BG));
    draw_topbar("ACCUEIL");
    fill_rect(14, 61, 452, 62, panel);
    fill_rect(14, 61, 4, 62, C_ACCENT);
    draw_text(29, 75, "BONJOUR !", C_ACCENT, 2);
    draw_text(29, 101, "VOTRE PSP, UN NOUVEAU CHEZ-VOUS.", color(C_MUTED, RGBA(90,93,110,255)), 1);

    draw_card(14, 137, 218, 48, g_selection == 0, 'A',
              "APPLICATIONS", "Bibliotheque", RGBA(81,126,211,255));
    draw_card(248, 137, 218, 48, g_selection == 1, 'P',
              "PARAMETRES", "Personnaliser le systeme", RGBA(143,105,211,255));
    draw_card(14, 197, 218, 42, g_selection == 2, 'R',
              "RECOVERY", "Outils de secours", RGBA(211,143,76,255));
    draw_card(248, 197, 218, 42, g_selection == 3, 'X',
              "QUITTER", "Retourner au XMB", RGBA(75,170,143,255));
    draw_bottom_hint("CROIX : OUVRIR     D-PAD : NAVIGUER     CERCLE : QUITTER");
}

static void draw_settings(void)
{
    int i;
    const char *labels[] = {"APPARENCE", "REFAIRE LE PREMIER DEMARRAGE", "RETOUR"};
    const char *values[] = {0, "LANCER", "ACCUEIL"};
    fill_rect(0, 0, SCREEN_W, SCREEN_H, color(C_BG, C_LIGHT_BG));
    draw_topbar("PARAMETRES");
    fill_rect(14, 62, 452, 34, color(C_PANEL, RGBA(250,251,255,255)));
    draw_text(27, 75, "PERSONNALISATION DU SYSTEME", C_ACCENT, 1);
    for (i = 0; i < 3; ++i) {
        int y = 108 + i * 43;
        fill_rect(14, y, 452, 36, color(C_PANEL, RGBA(250,251,255,255)));
        if (g_selection == i) outline_rect(12, y - 2, 456, 40, C_ACCENT);
        draw_text(27, y + 14, labels[i], color(C_TEXT, RGBA(35,35,50,255)), 1);
        if (i == 0) {
            const char *theme = g_config.dark_theme ? "SOMBRE" : "CLAIR";
            fill_rect(360, y + 7, 90, 22, C_ACCENT_D);
            draw_text(377, y + 15, theme, C_TEXT, 1);
        } else {
            draw_text(390, y + 14, values[i], C_MUTED, 1);
        }
    }
    draw_bottom_hint("CROIX : VALIDER     CERCLE : RETOUR");
}

static void draw_recovery(void)
{
    fill_rect(0, 0, SCREEN_W, SCREEN_H, color(C_BG, C_LIGHT_BG));
    draw_topbar("RECOVERY");
    fill_rect(34, 68, 412, 105, color(C_PANEL, RGBA(250,251,255,255)));
    draw_centered(84, "MODE RECOVERY", C_ORANGE, 2);
    draw_centered(116, "L'INTERFACE PEUT ETRE RELANCEE ICI.", color(C_TEXT, RGBA(35,35,50,255)), 1);
    draw_centered(137, "LE FIRMWARE ET LE XMB RESTENT INTACTS.", C_MUTED, 1);
    fill_rect(68, 190, 344, 35, color(C_PANEL, RGBA(250,251,255,255)));
    outline_rect(66, 188, 348, 39, C_ACCENT);
    draw_centered(204, "CROIX : REVENIR A L'ACCUEIL", C_TEXT, 1);
    draw_bottom_hint("CERCLE : QUITTER VERS LE XMB");
}

static void draw_quick_setup(void)
{
    fill_rect(0, 0, SCREEN_W, SCREEN_H, color(C_BG, C_LIGHT_BG));
    draw_topbar("PREMIER DEMARRAGE");
    fill_rect(28, 66, 424, 145, color(C_PANEL, RGBA(250,251,255,255)));
    draw_centered(84, "BIENVENUE !", C_ACCENT, 2);
    draw_centered(119, "LINEAGEOS PSP EST PRET.", color(C_TEXT, RGBA(35,35,50,255)), 1);
    draw_centered(143, "APPARENCE :", C_MUTED, 1);
    draw_centered(163, g_config.dark_theme ? "SOMBRE" : "CLAIR", C_ACCENT, 1);
    draw_centered(190, "APPUYEZ SUR CROIX POUR CONTINUER", C_GREEN, 1);
    draw_bottom_hint("CONFIGURATION ENREGISTREE SUR LA MEMORY STICK");
}

static void render(void)
{
    if (!g_config.quick_setup_done) draw_quick_setup();
    else if (g_screen == SCREEN_SETTINGS) draw_settings();
    else if (g_screen == SCREEN_RECOVERY) draw_recovery();
    else draw_home();
}

static void present(void)
{
    sceDisplayWaitVblankStart();
    sceDisplaySetFrameBuf(g_back, FB_STRIDE, PSP_DISPLAY_PIXEL_FORMAT_8888,
                          PSP_DISPLAY_SETBUF_NEXTFRAME);
    {
        unsigned int *tmp = g_front;
        g_front = g_back;
        g_back = tmp;
    }
}

static void status_set(const char *message)
{
    strncpy(g_status, message, sizeof(g_status) - 1);
    g_status[sizeof(g_status) - 1] = '\0';
}

static void handle_input(unsigned int pressed)
{
    if (!g_config.quick_setup_done) {
        if (pressed & PSP_CTRL_CROSS) {
            g_config.quick_setup_done = 1;
            config_save(&g_config);
            g_screen = SCREEN_HOME;
            g_selection = 0;
        }
        return;
    }

    if (g_screen == SCREEN_HOME) {
        if (pressed & PSP_CTRL_UP) g_selection = (g_selection < 2) ? g_selection + 2 : g_selection - 2;
        else if (pressed & PSP_CTRL_DOWN) g_selection = (g_selection + 2) % 4;
        else if (pressed & PSP_CTRL_LEFT) g_selection = (g_selection % 2 == 1) ? g_selection - 1 : g_selection;
        else if (pressed & PSP_CTRL_RIGHT) g_selection = (g_selection % 2 == 0) ? g_selection + 1 : g_selection;
        else if (pressed & PSP_CTRL_CROSS) {
            if (g_selection == 0) status_set("La bibliotheque d'applications arrive plus tard.");
            else if (g_selection == 1) { g_screen = SCREEN_SETTINGS; g_selection = 0; }
            else if (g_selection == 2) g_screen = SCREEN_RECOVERY;
            else g_running = 0;
        } else if (pressed & PSP_CTRL_CIRCLE) g_running = 0;
    } else if (g_screen == SCREEN_SETTINGS) {
        if (pressed & PSP_CTRL_UP) g_selection = (g_selection + 2) % 3;
        else if (pressed & PSP_CTRL_DOWN) g_selection = (g_selection + 1) % 3;
        else if (pressed & PSP_CTRL_CROSS) {
            if (g_selection == 0) {
                g_config.dark_theme = !g_config.dark_theme;
                config_save(&g_config);
            } else if (g_selection == 1) g_config.quick_setup_done = 0;
            else { g_screen = SCREEN_HOME; g_selection = 1; }
        } else if (pressed & PSP_CTRL_CIRCLE) {
            g_screen = SCREEN_HOME;
            g_selection = 1;
        }
    } else if (g_screen == SCREEN_RECOVERY) {
        if (pressed & PSP_CTRL_CROSS) { g_screen = SCREEN_HOME; g_selection = 0; }
        else if (pressed & PSP_CTRL_CIRCLE) g_running = 0;
    }
}

int main(void)
{
    SceCtrlData pad;
    unsigned int previous = 0;
    unsigned int pressed;
    int i;

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);
    sceDisplaySetMode(0, SCREEN_W, SCREEN_H);
    sceDisplaySetFrameBuf(g_back, FB_STRIDE, PSP_DISPLAY_PIXEL_FORMAT_8888,
                          PSP_DISPLAY_SETBUF_IMMEDIATE);

    config_set_defaults(&g_config);
    config_load(&g_config);

    for (i = 0; i < FB_SIZE; ++i) {
        g_back[i] = C_BG;
        g_front[i] = C_BG;
    }

    while (g_running) {
        sceCtrlReadBufferPositive(&pad, 1);
        pressed = pad.Buttons & ~previous;
        previous = pad.Buttons;
        handle_input(pressed);
        render();
        present();
        ++g_frame;
    }

    sceDisplaySetFrameBuf(NULL, 0, PSP_DISPLAY_PIXEL_FORMAT_8888,
                          PSP_DISPLAY_SETBUF_IMMEDIATE);
    sceKernelExitGame();
    return 0;
}
