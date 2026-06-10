"""
Katlans Code Generator  —  AST → C source
"""
import os
from .lexer import Lexer
from .parser import Parser
from .ast_nodes import *
from .errors import CodeGenError


# Map built-in function names to C runtime calls
BUILTIN_MAP = {
    # ── Math ─────────────────────────────────────
    "sqrt":   "k_sqrt",  "abs":    "k_abs",   "floor":  "k_floor",
    "ceil":   "k_ceil",  "log":    "k_log",   "ln":     "k_ln",
    "exp":    "k_exp",   "sin":    "k_sin",   "cos":    "k_cos",
    "tan":    "k_tan",   "arcsin": "k_asin",  "arccos": "k_acos",
    "arctan": "k_atan",  "sum":    "k_sum",   "len":    "k_len",
    "round":  "k_round", "PI":     "K_PI",    "E":      "K_E",
    "min":    "k_min2",  "max":    "k_max2",

    # ── String (S prefix) ────────────────────────
    "Scap":      "k_Scap",     "Slow":       "k_Slow",
    "Slen":      "k_Slen",     "Snip":       "k_Snip",
    "Sfind":     "k_Sfind",    "Scount":     "k_Scount",
    "Speeks":    "k_Speeks",   "Sfront":     "k_Sfront",
    "Sback":     "k_Sback",    "Sswap":      "k_Sswap",
    "Schop":     "k_Schop",    "Srchop":     "k_Srchop",
    "Schoplines":"k_Schoplines","Stitle":    "k_Stitle",
    "Scapfirst": "k_Scapfirst","Sswapcase":  "k_Sswapcase",
    "Scasefold": "k_Scasefold","Sremprefix": "k_Sremprefix",
    "Sremsuffix":"k_Sremsuffix","Szfill":   "k_Szfill",
    "Srfind":    "k_Srfind",   "Sindex":     "k_Sindex",
    "Srindex":   "k_Srindex",  "Sjoin":      "k_Sjoin",
    "Sisalpha":  "k_Sisalpha", "Sisdigit":   "k_Sisdigit",
    "Sisalnum":  "k_Sisalnum", "Sisspace":   "k_Sisspace",
    "Sislower":  "k_Sislower", "Sisupper":   "k_Sisupper",
    "Sistitle":  "k_Sistitle", "Sisascii":   "k_Sisascii",

    # ── List (L prefix) ──────────────────────────
    "Ladd":    "k_Ladd",    "Llen":   "k_Llen",   "Lmax":   "k_Lmax",
    "Lmin":    "k_Lmin",    "Lsum":   "k_sum",    "Lget":   "k_Lget",
    "Lsort":   "k_Lsort",   "Lflip":  "k_Lflip",  "Ldrop":  "k_Ldrop",
    "Lpop":    "k_Lpop",    "Lindex": "k_Lindex", "Lcount": "k_Lcount",
    "Lhas":    "k_Lhas",    "Lcopy":  "k_Lcopy",  "Lclear": "k_Lclear",
    "Linsert": "k_Linsert", "Lextend":"k_Lextend","Lunique":"k_Lunique",
    "Lslice":  "k_Lslice",  "Lreversed":"k_Lreversed","Lsorted":"k_Lsorted",

    # ── Adero/Dict (A prefix) ─────────────────────
    "Alen":    "k_Alen",    "Akeys":  "k_Akeys",  "Avals":  "k_Avals",
    "Aitems":  "k_Aitems",  "Aget":   "k_Aget",   "Ahas":   "k_Ahas",
    "Aset":    "k_Aset",    "Adrop":  "k_Adrop",  "Aclear": "k_Aclear",
    "Acopy":   "k_Acopy",

    # ── Zelo/Tuple (Z prefix) ─────────────────────
    "Zlen":   "k_Zlen",    "Zmax":  "k_Zmax",    "Zmin":  "k_Zmin",
    "Zsum":   "k_Zsum",    "Zhas":  "k_Zhas",    "Zindex":"k_Zindex",
    "Zcount": "k_Zcount",

    # ── Global built-ins ───────────────────────────
    "type":   "k_type",    "chr":    "k_chr",      "ord":   "k_ord",
    "hex":    "k_hex",     "oct":    "k_oct",      "bin":   "k_bin",
    "divmod": "k_divmod",  "all":    "k_all",      "any":   "k_any",
    "gcd":    "k_gcd",     "lcm":    "k_lcm",      "fact":  "k_fact",
    "rand":   "k_rand",    "enumerate":"k_ltenum","zip":   "k_ltzip",

    # ── Cast ──────────────────────────────────────
    "I":      "k_I",       "F":      "k_F",        "S":     "k_S",

    # ── File I/O (f prefix) ───────────────────────
    "fread":    "k_fread",    "fwrite":    "k_fwrite",
    "fadd":     "k_fadd",     "fdel":      "k_fdel",
    "fexists":  "k_fexists",  "flist":     "k_flist",
    "fmkdir":   "k_fmkdir",   "frmdir":    "k_frmdir",
    "frename":  "k_frename",  "fcopy":     "k_fcopy",
    "fsize":    "k_fsize",    "fisdir":    "k_fisdir",
    "fisfile":  "k_fisfile",  "fext":      "k_fext",
    "fbase":    "k_fbase",    "fdir":      "k_fdir",
    "fjoin":    "k_fjoin",    "freadlines":"k_freadlines",
    "fwritelines":"k_fwritelines",

    # ── OS (os prefix) ────────────────────────────
    "ossys":    "k_ossys",    "osram":     "k_osram",
    "osuptime": "k_osuptime", "osboot":    "k_osboot",
    "osuser":   "k_osuser",   "osrun":     "k_osrun",
    "osget":    "k_osget",    "onset":     "k_onset",
    "ospwd":    "k_ospwd",    "oscd":      "k_oscd",
    "osjoin":   "k_osjoin",   "osbase":    "k_osbase",
    "osdir":    "k_osdir",    "osext":     "k_osext",
    "ospath":   "k_ospath",   "osopen":    "k_osopen",
    "osclipget":"k_osclipget","osclipset": "k_osclipset",
    "osnoti":   "k_osnoti",   "oshalt":    "k_oshalt",

    # ── Regex (rx prefix) ─────────────────────────
    "rxmatch":  "k_rxmatch",  "rxfind":    "k_rxfind",
    "rxswap":   "k_rxswap",   "rxchop":    "k_rxchop",
    "rxgroups": "k_rxgroups", "rxfull":    "k_rxfull",
    "rxcount":  "k_rxcount",

    # ── Data Formats (js/csv/xml) ─────────────────
    "jsparse":  "k_jsparse",  "jsstring":  "k_jsstring",
    "jsread":   "k_jsread",   "jswrite":   "k_jswrite",
    "csvread":  "k_csvread",  "csvwrite":  "k_csvwrite",
    "csvheads": "k_csvheads", "csvparse":  "k_csvparse",
    "xmlparse": "k_xmlparse", "xmlget":    "k_xmlget",
    "xmlattr":  "k_xmlattr",  "xmlstring": "k_xmlstring",

    # ── Crypto (cr prefix) ────────────────────────
    "crmd5":    "k_crmd5",    "crsha":     "k_crsha",
    "crhash":   "k_crhash",   "crb64enc":  "k_crb64enc",
    "crb64dec": "k_crb64dec", "cruuid":    "k_cruuid",
    "crtoken":  "k_crtoken",  "crenc":     "k_crenc",
    "crdec":    "k_crdec",    "crcomp":    "k_crcomp",

    # ── CLI Tools (cli prefix) ─────────────────────
    "clired":     "k_clired",     "cligreen":     "k_cligreen",
    "cliyellow":  "k_cliyellow",  "clibold":      "k_clibold",
    "cliblue":    "k_cliblue",    "climagenta":   "k_climagenta",
    "clicyan":    "k_clicyan",    "clidim":       "k_clidim",
    "cliunder":   "k_cliunder",   "climenu":      "k_climenu",
    "cliargs":    "k_cliargs",    "cliflag":      "k_cliflag",
    "cliopt":     "k_cliopt",     "cliprogress":  "k_cliprogress",
    "clipbset":   "k_clipbset",   "clipbdone":    "k_clipbdone",

    # ── Testing (test prefix) ──────────────────────
    "testcheck":  "k_testcheck",

    # ── Date & Time (dt prefix) ────────────────────
    "dtnow":     "k_dtnow",      "dtdate":    "k_dtdate",
    "dttime":    "k_dttime",     "dtformat":  "k_dtformat",
    "dtparse":   "k_dtparse",    "dtstamp":   "k_dtstamp",
    "dtfromstamp":"k_dtfromstamp","dtadd":   "k_dtadd",
    "dtsub":     "k_dtsub",      "dtdiff":   "k_dtdiff",
    "dtconv":    "k_dtconv",     "dtwait":   "k_dtwait",
    "dtleap":    "k_dtleap",     "dtbefore": "k_dtbefore",
    "dtafter":   "k_dtafter",    "dtequal":  "k_dtequal",
    "dtzone":    "k_dtzone",

    # ── Networking (net prefix) ────────────────────
    "netget":    "k_netget",     "netpost":  "k_netpost",
    "netput":    "k_netput",     "netpatch": "k_netpatch",
    "netdel":    "k_netdel",     "netjson":  "k_netjson",
    "netdns":    "k_netdns",     "netping":  "k_netping",
    "netdl":     "k_netdl",

    # ── Concurrency (cx prefix) ────────────────────
    "cxqueue":    "k_cxqueue",    "cxpush":   "k_cxpush",
    "cxpop":      "k_cxpop",      "cxatom":   "k_cxatom",
    "cxinc":      "k_cxinc",      "cxdec":    "k_cxdec",
    "cxlock":     "k_cxlock",     "cxacquire":"k_cxacquire",
    "cxrelease":  "k_cxrelease",  "cxfuture": "k_cxfuture",
    "cxresolve":  "k_cxresolve",  "cxchan":   "k_cxchan",
    "cxsend":     "k_cxsend",     "cxrecv":   "k_cxrecv",
    "cxwait":     "k_cxwait_secs","cxsem":    "k_cxsem",
    "cxretry":    "k_cxretry",    "cxtimeout":"k_cxtimeout",
    "cxpool":     "k_cxpool",     "cxsubmit": "k_cxsubmit",
    "cxcancel":   "k_cxcancel",   "cxabort":  "k_cxabort",

    # ── Data Structures (ds prefix) ─────────────────
    "dsstack":   "k_dsstack",    "dspush":    "k_dspush",
    "dspop":     "k_dspop",      "dspeek":    "k_dspeek",
    "dssize":    "k_dssize",     "dsqueue":   "k_dsqueue",
    "dsenqueue": "k_dsenqueue",  "dsdequeue": "k_dsdequeue",
    "dsset":     "k_dsset",      "dsadd":     "k_dsadd",
    "dsrem":     "k_dsrem",      "dshas":     "k_dshas",
    "dsunion":   "k_dsunion",    "dsinter":   "k_dsinter",
    "dsdiff":    "k_dsdiff",     "dsll":      "k_dsll",
    "dsllpush":  "k_dsllpush",   "dsllpop":   "k_dsllpop",
    "dsllget":   "k_dsllget",    "dstree":    "k_dstree",
    "dstadd":    "k_dstadd",     "dstchildren":"k_dstchildren",
    "dstparent": "k_dstparent",  "dsgraph":   "k_dsgraph",
    "dsgedge":   "k_dsgedge",    "dsgnbr":    "k_dsgnbr",
    "dsgpath":   "k_dsgpath",    "dsheap":    "k_dsheap",
    "dshpush":   "k_dshpush",    "dshpop":    "k_dshpop",
    "dsdeque":   "k_dsdeque",    "dqdpushl":  "k_dqdpushl",
    "dqdpushr":  "k_dqdpushr",   "dqdpopl":   "k_dqdpopl",
    "dqdpopr":   "k_dqdpopr",    "dsmat":     "k_dsmat",
    "dsmatset":  "k_dsmatset",   "dsmatget":  "k_dsmatget",
    "dsmatmul":  "k_dsmatmul",   "dsmattrans":"k_dsmattrans",
    "dsmatprint":"k_dsmatprint",

    # ── Zip Compression (zp prefix) ───────────────
    "zppack":   "k_zppack",    "zpunpack": "k_zpunpack",
    "zplist":   "k_zplist",    "zpadd":    "k_zpadd",
    "zprem":    "k_zprem",

    # ── Serialization (ser prefix) ────────────────
    "serencode":    "k_serencode",    "serdecode":  "k_serdecode",
    "serbytes":     "k_serbytes",     "serfrombytes":"k_serfrombytes",

    # ── Phase 5: ML Module (ml prefix) ───────────
    "mlzeros":      "k_mlzeros",      "mlones":        "k_mlones",
    "mlrand":       "k_mlrand",       "mlrandn":       "k_mlrandn",
    "mltensor_from_list":"k_mltensor_from_list","mltensor_shape":"k_mltensor_shape",
    "mlt_sum":      "k_mlt_sum",      "mlt_mean":      "k_mlt_mean",
    "mlt_max":      "k_mlt_max",      "mlt_min":       "k_mlt_min",
    "mlt_add":      "k_mlt_add",      "mlt_sub":       "k_mlt_sub",
    "mlt_mul":      "k_mlt_mul",      "mlt_matmul":    "k_mlt_matmul",
    "mlt_relu":     "k_mlt_relu",     "mlt_sigmoid":   "k_mlt_sigmoid",
    "mlt_softmax":  "k_mlt_softmax",  "mlt_tanh":      "k_mlt_tanh",
    "mlt_flatten":  "k_mlt_flatten",  "mlt_transpose": "k_mlt_transpose",
    "mlt_tolist":   "k_mlt_tolist",   "mlt_get":       "k_mlt_get",
    "mlt_set":      "k_mlt_set",      "mlt_print":     "k_mlt_print",
    "mlmodel_seq":  "k_mlmodel_seq",  "mlmodel_add_linear":"k_mlmodel_add_linear",
    "mlmodel_add_relu":"k_mlmodel_add_relu","mlmodel_add_sigmoid":"k_mlmodel_add_sigmoid",
    "mlmodel_add_softmax":"k_mlmodel_add_softmax","mlmodel_forward":"k_mlmodel_forward",
    "mlmodel_predict":"k_mlmodel_predict","mlmodel_predict_class":"k_mlmodel_predict_class",
    "mlmodel_save": "k_mlmodel_save",
    "mlloss_mse":   "k_mlloss_mse",   "mlloss_bce":    "k_mlloss_bce",
    "mlloss_crossentropy":"k_mlloss_crossentropy","mlloss_mae":"k_mlloss_mae",
    "mlmetric_accuracy":"k_mlmetric_accuracy","mlmetric_r2":"k_mlmetric_r2",
    "ml_gpu_available":"k_ml_gpu_available",
    "mlt_gpu":      "k_mlt_gpu",      "mlt_cpu":       "k_mlt_cpu",

    # ── Phase 5: Finance Module (fin prefix) ─────
    "finticker":    "k_finticker",    "finprice":      "k_finprice",
    "finhist":      "k_finhist",      "fininfo":       "k_fininfo",
    "finind_sma":   "k_finind_sma",   "finind_ema":    "k_finind_ema",
    "finind_rsi":   "k_finind_rsi",   "finind_macd":   "k_finind_macd",
    "finind_bollinger":"k_finind_bollinger","finind_atr":"k_finind_atr",
    "finind_obv":   "k_finind_obv",   "finind_vwap":   "k_finind_vwap",
    "finchart_line":"k_finchart_line","finchart_candle":"k_finchart_candle",
    "finchart_portfolio":"k_finchart_portfolio","finchart_show":"k_finchart_show",
    "finchart_save":"k_finchart_save","finchart_compare":"k_finchart_compare",
    "finport_create":"k_finport_create","finport_add":"k_finport_add",
    "finport_value":"k_finport_value","finport_cost":"k_finport_cost",
    "finport_pnl":  "k_finport_pnl",  "finport_pnl_pct":"k_finport_pnl_pct",
    "finport_save": "k_finport_save",  "finport_load": "k_finport_load",
    "finback_create":"k_finback_create","finback_cash":"k_finback_cash",
    "finback_commission":"k_finback_commission","finback_buy":"k_finback_buy",
    "finback_sell": "k_finback_sell",  "finback_run":  "k_finback_run",
    "finmarket_overview":"k_finmarket_overview",
    "finforex":     "k_finforex",     "fincrypto_top": "k_fincrypto_top",
    "finpat_doji":  "k_finpat_doji",  "finpat_hammer":"k_finpat_hammer",

    # ── Phase 5: Game Module (gm prefix) ─────────
    "gm_init":      "k_gm_init",      "gmwin":         "k_gmwin",
    "gmwin_clear":  "k_gmwin_clear",  "gmwin_flip":    "k_gmwin_flip",
    "gmwin_caption":"k_gmwin_caption","gmwin_fps":    "k_gmwin_fps",
    "gmwin_hdc":    "k_gmwin_hdc",    "gmwin_size":   "k_gmwin_size",
    "gm_running":   "k_gm_running",   "gm_stop":       "k_gm_stop",
    "gm_quit":      "k_gm_quit",      "gm_events":     "k_gm_events",
    "gminput_key_pressed":"k_gminput_key_pressed","gminput_mouse_pos":"k_gminput_mouse_pos",
    "gmdraw_rect":  "k_gmdraw_rect",  "gmdraw_circle": "k_gmdraw_circle",
    "gmdraw_line":  "k_gmdraw_line",  "gmdraw_text":   "k_gmdraw_text",
    "gmdraw_point": "k_gmdraw_point",
    "gmsprite":     "k_gmsprite",     "gmsp_pos":      "k_gmsp_pos",
    "gmsp_x":       "k_gmsp_x",       "gmsp_y":        "k_gmsp_y",
    "gmsp_vel":     "k_gmsp_vel",     "gmsp_move":     "k_gmsp_move",
    "gmsp_moveto":  "k_gmsp_moveto",  "gmsp_visible":  "k_gmsp_visible",
    "gmsp_accel":   "k_gmsp_accel",   "gmsp_update":   "k_gmsp_update",
    "gmsp_draw":    "k_gmsp_draw",    "gmsp_kill":     "k_gmsp_kill",
    "gmsp_rect":    "k_gmsp_rect",
    "gmgroup":      "k_gmgroup",      "gmgroup_add":   "k_gmgroup_add",
    "gmgroup_remove":"k_gmgroup_remove","gmgroup_update":"k_gmgroup_update",
    "gmgroup_draw": "k_gmgroup_draw", "gmgroup_clear": "k_gmgroup_clear",
    "gmgroup_count":"k_gmgroup_count",
    "gmclock":      "k_gmclock",      "gmclock_tick":  "k_gmclock_tick",
    "gmcol_rect":   "k_gmcol_rect",   "gmcol_rect_point":"k_gmcol_rect_point",
    "gmcol_circle": "k_gmcol_circle", "gmcol_group":   "k_gmcol_group",
    "gmcol_dist":   "k_gmcol_dist",
    "gmsound":      "k_gmsound",      "gmsound_play":  "k_gmsound_play",
    "gmsound_stop": "k_gmsound_stop", "gmsound_vol":   "k_gmsound_vol",
    "gmmusic_play": "k_gmmusic_play", "gmmusic_stop":  "k_gmmusic_stop",
    "gmfont":       "k_gmfont",       "gmfont_render": "k_gmfont_render",
    "gmtext_draw":  "k_gmtext_draw",
    "gmcam":        "k_gmcam",        "gmcam_follow":  "k_gmcam_follow",
    "gmcam_pos":    "k_gmcam_pos",    "gmcam_zoom":    "k_gmcam_zoom",
    "gmstate_save": "k_gmstate_save", "gmstate_load": "k_gmstate_load",
    "gmstate_exists":"k_gmstate_exists","gm_event_post":"k_gm_event_post",

    # ── Phase 5: UI Module (ui prefix) ───────────
    "uiwin":        "k_uiwin",        "uiwin_run":     "k_uiwin_run",
    "uiwin_close":  "k_uiwin_close",  "uiwin_title":   "k_uiwin_title",
    "uiwin_flip":   "k_uiwin_flip",   "uiwin_center":  "k_uiwin_center",
    "uirun":        "k_uirun",
    "uilabel":      "k_uilabel",      "uilabel_text":  "k_uilabel_text",
    "uibtn":        "k_uibtn",
    "uientry":      "k_uientry",      "uientry_get":   "k_uientry_get",
    "uientry_set":  "k_uientry_set",  "uientry_clear": "k_uientry_clear",
    "uitext":       "k_uitext",       "uitext_insert": "k_uitext_insert",
    "uitext_get":   "k_uitext_get",   "uitext_clear":  "k_uitext_clear",
    "uicheck":      "k_uicheck",      "uicheck_get":   "k_uicheck_get",
    "uicheck_set":  "k_uicheck_set",  "uicheck_toggle":"k_uicheck_toggle",
    "uiprog":       "k_uiprog",       "uiprog_set":    "k_uiprog_set",
    "uislider":     "k_uislider",     "uislider_get":  "k_uislider_get",
    "uislider_set": "k_uislider_set",
    "uipack":       "k_uipack",       "uigrid":        "k_uigrid",
    "uiplace":      "k_uiplace",
    "uidialog_info":"k_uidialog_info","uidialog_ask": "k_uidialog_ask",
    "uidialog_input":"k_uidialog_input",
    "uiclip_set":   "k_uiclip_set",   "uiclip_get":    "k_uiclip_get",
    "uivar_str":    "k_uivar_str",    "uivar_int":     "k_uivar_int",
    "uivar_bool":   "k_uivar_bool",   "uivar_get":     "k_uivar_get",
    "uivar_set":    "k_uivar_set",
    "uiinfo_screenwidth":"k_uiinfo_screenwidth","uiinfo_screenheight":"k_uiinfo_screenheight",

    # ── Design Module (ds prefix) ────────────────
    "ds_color":        "k_ds_color",        "ds_color_rgb":    "k_ds_color_rgb",
    "ds_color_hex":    "k_ds_color_hex",    "ds_bind":         "k_ds_bind",
    "ds_size":         "k_ds_size",         "ds_draw_roundrect":"k_ds_draw_roundrect",
    "ds_fill_rect":    "k_ds_fill_rect",     "ds_draw_rect":    "k_ds_draw_rect",
    "ds_gradient":     "k_ds_gradient",      "ds_shadow":       "k_ds_shadow",
    "ds_text":         "k_ds_text",          "ds_text_bold":    "k_ds_text_bold",
    "ds_text_measure": "k_ds_text_measure",  "ds_parse_css":    "k_ds_parse_css",
    "ds_parse_styles": "k_ds_parse_styles",  "ds_box_model":    "k_ds_box_model",
    "ds_layout_flex":  "k_ds_layout_flex",   "ds_layout_grid":  "k_ds_layout_grid",
    "ds_apply_style":  "k_ds_apply_style",   "ds_render_box":   "k_ds_render_box",
    "ds_screen_size":  "k_ds_screen_size",

    # ── Phase 5: VIS Module (vis prefix) ─────────
    "viscam":           "k_viscam",           "viscam_list":         "k_viscam_list",
    "viscam_open":       "k_viscam_open",       "viscam_close":        "k_viscam_close",
    "viscam_width":      "k_viscam_width",      "viscam_height":       "k_viscam_height",
    "visframe":          "k_visframe",          "visframe_blank":      "k_visframe_blank",
    "visload":           "k_visload",           "vissave":             "k_vissave",
    "visframe_show":     "k_visframe_show",     "visframe_size":       "k_visframe_size",
    "visframe_gray":     "k_visframe_gray",     "visframe_rgb":        "k_visframe_rgb",
    "visframe_blur":     "k_visframe_blur",     "visframe_threshold":  "k_visframe_threshold",
    "visframe_contours": "k_visframe_contours", "visframe_equalize":   "k_visframe_equalize",
    "visframe_flip":     "k_visframe_flip",     "visframe_rotate":     "k_visframe_rotate",
    "visframe_resize":   "k_visframe_resize",   "visframe_crop":       "k_visframe_crop",
    "visframe_histogram":"k_visframe_histogram","visframe_edges_canny":"k_visframe_edges_canny",
    "vis_facedetect":    "k_vis_facedetect",    "vis_facemesh":        "k_vis_facemesh",
    "visface_add":       "k_visface_add",       "visface_remove":      "k_visface_remove",
    "visface_load":      "k_visface_load",      "visface_save":        "k_visface_save",
    "vis_facerecog":     "k_vis_facerecog",     "vis_landmarks":       "k_vis_landmarks",
    "vis_hands":         "k_vis_hands",         "vis_pose":            "k_vis_pose",
    "vis_objects":       "k_vis_objects",       "vis_segment":         "k_vis_segment",
    "vis_eyes":          "k_vis_eyes",          "vis_emotion":         "k_vis_emotion",
    "vis_agegender":     "k_vis_agegender",     "vis_motion":          "k_vis_motion",
    "vis_flow":          "k_vis_flow",          "vis_color":           "k_vis_color",
    "vis_qr":            "k_vis_qr",            "visocr":              "k_visocr",
    "vis_plate":         "k_vis_plate",         "vistemplate":         "k_vistemplate",
    "visfeature_detect": "k_visfeature_detect",
    "visdraw_box":       "k_visdraw_box",       "visdraw_circle":      "k_visdraw_circle",
    "visdraw_line":      "k_visdraw_line",      "visdraw_text":        "k_visdraw_text",
    "visrec_start":      "k_visrec_start",      "visrec_frame":        "k_visrec_frame",
    "visrec_stop":       "k_visrec_stop",       "visshot":             "k_visshot",
    "visvid":            "k_visvid",            "visvid_fps":          "k_visvid_fps",
    "visvid_framecount": "k_visvid_framecount", "visvid_duration":     "k_visvid_duration",
    "visvid_read":       "k_visvid_read",       "visvid_close":        "k_visvid_close",
    "visvid_seek":       "k_visvid_seek",       "visvid_writer":       "k_visvid_writer",
    "visvid_write":      "k_visvid_write",      "visvid_close_writer": "k_visvid_close_writer",
}

# Built-ins that return void (modify in place) — no KVal* assignment
VOID_BUILTINS = {
    "Ladd", "Lsort", "Lflip", "Ldrop", "Lclear", "Linsert", "Lextend",
    "Aset", "Adrop", "Aclear",
    "dsadd", "dsrem", "dspush", "dsenqueue",
    "dshpush", "dsmatset", "dsgedge", "dstadd",
    "cxpush", "cxinc", "cxdec", "cxacquire", "cxrelease",
    "cxsend", "dsllpush",
    "dqdpushl", "dqdpushr",
    "fwrite", "fadd", "fdel", "fmkdir", "frmdir",
    "onset", "oscd", "osrun", "osopen", "osclipset", "osnoti",
    "jswrite", "csvwrite",

    # ── Phase 5 void built-ins ────────────────────
    "mlt_set", "mlt_print",
    "mlmodel_add_linear", "mlmodel_add_relu", "mlmodel_add_sigmoid", "mlmodel_add_softmax",
    "finport_add", "finchart_line", "finchart_candle", "finchart_portfolio", "finchart_show", "finchart_save", "finchart_compare",
    "finback_cash", "finback_commission", "finback_buy", "finback_sell",
    "viscam_open", "viscam_close", "vissave",
    "visframe_show", "visframe_gray", "visframe_rgb", "visframe_blur",
    "visframe_threshold", "visframe_equalize", "visframe_flip", "visframe_rotate", "visframe_resize", "visframe_crop",
    "visframe_edges_canny",
    "visface_add", "visface_remove", "visface_load", "visface_save",
    "visrec_start", "visrec_frame", "visrec_stop", "visshot",
    "visvid_close", "visvid_seek", "visvid_write", "visvid_close_writer",
    "visdraw_box", "visdraw_circle", "visdraw_line", "visdraw_text",
    "gmwin_clear", "gmwin_flip", "gmwin_caption", "gmwin_fps",
    "gm_stop", "gm_quit",
    "gmdraw_rect", "gmdraw_circle", "gmdraw_line", "gmdraw_text", "gmdraw_point",
    "gmsp_pos", "gmsp_x", "gmsp_y", "gmsp_vel", "gmsp_move", "gmsp_moveto",
    "gmsp_visible", "gmsp_accel", "gmsp_draw", "gmsp_kill",
    "gmgroup_add", "gmgroup_remove", "gmgroup_update", "gmgroup_draw", "gmgroup_clear",
    "gmsound_play", "gmsound_stop", "gmsound_vol",
    "gmmusic_play", "gmmusic_stop",
    "gmcam_follow", "gmcam_pos", "gmcam_zoom",
    "gm_event_post",
    "uiwin_close", "uiwin_title", "uiwin_flip", "uiwin_center",
    "uilabel_text", "uientry_set", "uientry_clear",
    "uitext_insert", "uitext_clear",
    "uicheck_set", "uicheck_toggle",
    "uiprog_set", "uislider_set",
    "uipack", "uigrid", "uiplace",
    "uiclip_set", "uivar_set",
}


class CodeGen:
    def __init__(self, runtime_path: str, filename: str = "<stdin>", root_dir: str = ""):
        self.runtime   = runtime_path
        self.filename  = filename
        self._root_dir = root_dir or os.path.dirname(os.path.abspath(filename)) if filename and filename != "<stdin>" else ""
        self._indent   = 0
        self._lines: list[str] = []
        self._func_names: set[str] = set()
        self._forward_decls: list[str] = []
        # Track declared variable names per scope to emit KVal* on first use
        self._declared: list[set] = [set()]   # stack of scopes
        # Track class types for variables (e.g., "c" -> "Counter")
        self._class_types: dict[str, str] = {}
        # Counter for template temp variable names
        self._template_counter = 0
        # Track already-included files to prevent circular deps
        self._included: set[str] = set()

    def _scope_push(self):  self._declared.append(set())
    def _scope_pop(self):   self._declared.pop()

    def _is_declared(self, name: str) -> bool:
        for scope in reversed(self._declared):
            if name in scope: return True
        return False

    def _declare(self, name: str):
        self._declared[-1].add(name)

    # ── Output helpers ────────────────────────────────────────────────────

    def _emit(self, line: str = ""):
        self._lines.append("    " * self._indent + line)

    def _emit_raw(self, line: str):
        self._lines.append(line)

    def _indent_in(self):  self._indent += 1
    def _indent_out(self): self._indent -= 1

    def _gen_block(self, body: list):
        self._scope_push()
        for s in body:
            self._gen_stmt(s)
        self._scope_pop()

    def _error(self, msg: str, node=None):
        line = getattr(node, "line", None)
        col  = getattr(node, "col",  None)
        raise CodeGenError(msg, line, col, self.filename)

    def generate(self, program: Program) -> str:
        # Preamble
        self._emit_raw(f'#include "{self.runtime}"')
        self._emit_raw("")

        # Collect forward declarations for user functions (direct + from includes)
        self._collect_funcs(program.body)

        # Emit forward declarations
        for fd in self._forward_decls:
            self._emit_raw(fd)
        if self._forward_decls:
            self._emit_raw("")

        # Pre-process UseStmt nodes: resolve includes and store body for later
        for node in program.body:
            if isinstance(node, UseStmt):
                self._gen_use_prepare(node)

        # Emit all function declarations (direct + from UseStmt bodies)
        for node in program.body:
            if isinstance(node, FuncDecl):
                self._gen_func_decl(node)
            elif isinstance(node, UseStmt):
                for n in node.body:
                    if isinstance(n, FuncDecl):
                        self._gen_func_decl(n)

        # Emit all class declarations
        for node in program.body:
            if isinstance(node, ClassDecl):
                self._gen_class_decl(node)
            elif isinstance(node, UseStmt):
                for n in node.body:
                    if isinstance(n, ClassDecl):
                        self._gen_class_decl(n)

        # Main function — emit only statements (not func/class decls)
        self._emit_raw("int main(void) {")
        self._indent_in()

        for node in program.body:
            if not isinstance(node, (FuncDecl, ClassDecl)):
                if isinstance(node, UseStmt):
                    # Emit only non-func/class statements from included file
                    for n in node.body:
                        if not isinstance(n, (FuncDecl, ClassDecl)):
                            self._gen_stmt(n)
                else:
                    self._gen_stmt(node)

        self._emit("return 0;")
        self._indent_out()
        self._emit_raw("}")

        return "\n".join(self._lines)

    # ── Forward declarations ───────────────────────────────────────────────

    def _collect_funcs(self, body: list):
        for node in body:
            if isinstance(node, FuncDecl):
                self._func_names.add(node.name)
                params = ", ".join(["KVal*"] * len(node.params))
                self._forward_decls.append(f"KVal* kf_{node.name}({params});")

    # ── Statement dispatch ─────────────────────────────────────────────────

    def _gen_stmt(self, node):
        if isinstance(node, VarDecl):
            self._gen_var_decl(node)
        elif isinstance(node, Assignment):
            self._gen_assignment(node)
        elif isinstance(node, DisplayStmt):
            self._gen_display(node)
        elif isinstance(node, IfStmt):
            self._gen_if(node)
        elif isinstance(node, ForRangeStmt):
            self._gen_for_range(node)
        elif isinstance(node, ForEachStmt):
            self._gen_for_each(node)
        elif isinstance(node, WhileStmt):
            self._gen_while(node)
        elif isinstance(node, HaltStmt):
            self._emit("break;")
        elif isinstance(node, ContinueStmt):
            self._emit("continue;")
        elif isinstance(node, ReturnStmt):
            self._gen_return(node)
        elif isinstance(node, FuncCall):
            # statement-level function call
            args_c = ", ".join(self._gen_expr(a) for a in node.args)
            if node.obj:
                # Method call
                self._emit(f"{self._gen_method_call(node)};")
            else:
                self._emit(f"kf_{node.name}({args_c});")
        elif isinstance(node, BuiltinCall):
            if node.func == "testcheck":
                args_c = ", ".join(self._gen_expr(a) for a in node.args)
                self._emit(f"k_testcheck({args_c}, \"<expr>\");")
            else:
                c = BUILTIN_MAP.get(node.func)
                if c:
                    args_c = ", ".join(self._gen_expr(a) for a in node.args)
                    self._emit(f"{c}({args_c});")
        elif isinstance(node, TestDecl):
            # Emit test body inline (tests execute in main)
            self._emit(f"/* test: {node.name} */")
            for s in node.body:
                self._gen_stmt(s)
        elif isinstance(node, UseStmt):
            self._gen_use(node)
        elif isinstance(node, TryCatch):
            self._gen_try(node)
        elif isinstance(node, ClassInstantiate):
            self._gen_class_inst(node)
        elif isinstance(node, FuncDecl):
            pass   # already emitted above main()
        elif isinstance(node, ClassDecl):
            pass
        elif node is None:
            pass
        else:
            # expression as statement
            self._emit(f"{self._gen_expr(node)};")

    # ── Variable declaration ──────────────────────────────────────────────

    def _gen_var_decl(self, node: VarDecl):
        val = self._gen_expr(node.value)
        self._declare(node.name)
        if node.dtype == "lt":
            if isinstance(node.value, ListLiteral):
                elems = node.value.elements
                # Single element that is a call returning a list → assign directly
                if (len(elems) == 1 and
                        isinstance(elems[0], (BuiltinCall, FuncCall))):
                    ev = self._gen_expr(elems[0])
                    self._emit(f"KVal* {node.name} = {ev};")
                else:
                    self._emit(f"KVal* {node.name} = kv_list();")
                    for el in elems:
                        ev = self._gen_expr(el)
                        self._emit(f"k_Ladd({node.name}, {ev});")
            else:
                # e.g. lt copy = Lcopy(x)  — just assign the returned list
                self._emit(f"KVal* {node.name} = {val};")
        elif node.dtype == "zl":
            if isinstance(node.value, TupleLiteral):
                self._emit(f"KVal* {node.name} = kv_list();")
                for el in node.value.elements:
                    ev = self._gen_expr(el)
                    self._emit(f"k_Ladd({node.name}, {ev});")
            else:
                self._emit(f"KVal* {node.name} = {val};")
        elif node.dtype == "ad":
            if isinstance(node.value, DictLiteral):
                self._emit(f"KVal* {node.name} = kv_dict();")
                for key, val_node in node.value.pairs:
                    kv = self._gen_expr(key)
                    vv = self._gen_expr(val_node)
                    # key must be a string
                    if isinstance(key, StrLiteral):
                        self._emit(f'kdict_set({node.name}->dict, {kv}->s, {vv});')
                    else:
                        self._emit(f'kdict_set({node.name}->dict, k_S({kv})->s, {vv});')
            else:
                self._emit(f"KVal* {node.name} = {val};")
        else:
            self._emit(f"KVal* {node.name} = {val};")

    # ── Assignment ────────────────────────────────────────────────────────

    def _gen_assignment(self, node: Assignment):
        val = self._gen_expr(node.value)
        if isinstance(node.target, Identifier):
            name = node.target.name
            if not self._is_declared(name):
                self._declare(name)
                self._emit(f"KVal* {name} = {val};")
            else:
                self._emit(f"{name} = {val};")
        elif isinstance(node.target, MemberAccess):
            obj = self._gen_expr(node.target.obj)
            self._emit(f'kdict_set({obj}->dict, "{node.target.member}", {val});')
        else:
            self._emit(f"{self._gen_expr(node.target)} = {val};")

    # ── Display ───────────────────────────────────────────────────────────

    def _gen_display(self, node: DisplayStmt):
        val = self._gen_expr(node.value)
        self._emit(f"k_display({val});")

    # ── If ────────────────────────────────────────────────────────────────

    def _gen_if(self, node: IfStmt):
        cond = self._gen_expr(node.condition)
        self._emit(f"if (k_truthy({cond})) {{")
        self._indent_in()
        self._gen_block(node.then_body)
        self._indent_out()
        self._emit("}")
        for ec, eb in node.elif_clauses:
            econd = self._gen_expr(ec)
            self._emit(f"else if (k_truthy({econd})) {{")
            self._indent_in()
            self._gen_block(eb)
            self._indent_out()
            self._emit("}")
        if node.else_body:
            self._emit("else {")
            self._indent_in()
            self._gen_block(node.else_body)
            self._indent_out()
            self._emit("}")

    # ── For range ─────────────────────────────────────────────────────────

    def _gen_for_range(self, node: ForRangeStmt):
        start = self._gen_expr(node.start)
        end   = self._gen_expr(node.end)
        step  = self._gen_expr(node.step)
        v     = node.var
        self._emit(f"for (long long _k_{v} = (long long)_knum({start});"
                   f" _k_{v} < (long long)_knum({end});"
                   f" _k_{v} += (long long)_knum({step})) {{")
        self._indent_in()
        self._emit(f"KVal* {v} = kv_int(_k_{v});")
        self._scope_push(); self._declare(v)
        for s in node.body: self._gen_stmt(s)
        self._scope_pop()
        self._indent_out()
        self._emit("}")

    def _gen_for_each(self, node: ForEachStmt):
        iterable = self._gen_expr(node.iterable)
        v = node.var
        idx = f"_ki_{v}"
        self._emit(f"for (size_t {idx} = 0; {idx} < {iterable}->list->len; {idx}++) {{")
        self._indent_in()
        self._emit(f"KVal* {v} = {iterable}->list->items[{idx}];")
        self._scope_push(); self._declare(v)
        for s in node.body: self._gen_stmt(s)
        self._scope_pop()
        self._indent_out()
        self._emit("}")

    def _gen_while(self, node: WhileStmt):
        cond = self._gen_expr(node.condition)
        self._emit(f"while (k_truthy({cond})) {{")
        self._indent_in()
        self._gen_block(node.body)
        self._indent_out()
        self._emit("}")

    # ── Return ────────────────────────────────────────────────────────────

    def _gen_return(self, node: ReturnStmt):
        if not node.values:
            self._emit("return kv_void();")
        elif len(node.values) == 1:
            self._emit(f"return {self._gen_expr(node.values[0])};")
        else:
            # Multiple return: pack into list
            self._emit("{ KVal* _ret = kv_list();")
            self._indent_in()
            for v in node.values:
                self._emit(f"k_Ladd(_ret, {self._gen_expr(v)});")
            self._emit("return _ret;")
            self._indent_out()
            self._emit("}")

    # ── Function declaration ───────────────────────────────────────────────

    def _gen_func_decl(self, node: FuncDecl):
        params_c = ", ".join(f"KVal* {p}" for p in node.params)
        self._emit_raw(f"KVal* kf_{node.name}({params_c}) {{")
        self._indent_in()
        self._scope_push()
        for p in node.params: self._declare(p)
        for op in node.out_params:
            self._emit(f"KVal* {op} = kv_void();")
            self._declare(op)
        for s in node.body:
            self._gen_stmt(s)
        self._scope_pop()
        self._emit("return kv_void();")
        self._indent_out()
        self._emit_raw("}")
        self._emit_raw("")

    # ── Class (simple struct-style) ───────────────────────────────────────

    def _gen_class_decl(self, node: ClassDecl):
        # Classes are represented as dicts at runtime
        # We emit a constructor function
        init_func = None
        methods = []
        props = []

        for item in node.body:
            if isinstance(item, FuncDecl) and item.name == "init":
                init_func = item
            elif isinstance(item, FuncDecl):
                methods.append(item)
            elif isinstance(item, VarDecl):
                props.append(item)

        # Emit constructor: KVal* kclass_Animal(KVal* name, KVal* age)
        params = []
        if init_func:
            params = init_func.params

        params_c = ", ".join(f"KVal* {p}" for p in params)
        self._emit_raw(f"KVal* kclass_{node.name}({params_c}) {{")
        self._indent_in()
        self._emit(f"KVal* self = kv_dict();")
        # Store class name in instance for method dispatch
        self._emit(f'kdict_set(self->dict, "_class", kv_str("{node.name}"));')

        # Default props
        for prop in props:
            pval = self._gen_expr(prop.value)
            self._emit(f'kdict_set(self->dict, "{prop.name}", {pval});')

        # Run init body (self is the object)
        if init_func:
            for s in init_func.body:
                self._gen_stmt_cls(s, "self")

        self._emit("return self;")
        self._indent_out()
        self._emit_raw("}")
        self._emit_raw("")

        # Emit methods
        for m in methods:
            mparams = ["KVal* self"] + [f"KVal* {p}" for p in m.params]
            self._emit_raw(f"KVal* kmethod_{node.name}_{m.name}({', '.join(mparams)}) {{")
            self._indent_in()
            for s in m.body:
                self._gen_stmt(s)
            self._emit("return kv_void();")
            self._indent_out()
            self._emit_raw("}")
            self._emit_raw("")

    def _gen_stmt_cls(self, node, self_var: str):
        """Generate stmt inside a class method — rewrite self.x = y"""
        if isinstance(node, Assignment) and isinstance(node.target, MemberAccess):
            if isinstance(node.target.obj, Identifier) and node.target.obj.name == "self":
                val = self._gen_expr(node.value)
                self._emit(f'kdict_set({self_var}->dict, "{node.target.member}", {val});')
                return
        self._gen_stmt(node)

    # ── Class instantiation ────────────────────────────────────────────────

    def _gen_class_inst(self, node: ClassInstantiate):
        args_c = ", ".join(self._gen_expr(a) for a in node.args)
        if node.var_name:
            self._emit(f"KVal* {node.var_name} = kclass_{node.cls_name}({args_c});")
            self._declare(node.var_name)
            self._class_types[node.var_name] = node.cls_name
        else:
            self._emit(f"kclass_{node.cls_name}({args_c});")

    # ── Try / Catch ───────────────────────────────────────────────────────

    def _gen_try(self, node: TryCatch):
        # C has no exceptions — wrap in a basic signal-free try block
        self._emit("/* try */ {")
        self._indent_in()
        for s in node.try_body:
            self._gen_stmt(s)
        self._indent_out()
        self._emit("}")
        # catch block (simplified — always runs for now in v1)
        self._emit(f"/* catch ({node.error_var}) */")

    # ── Use (file include) ──────────────────────────────────────────────────
    # use "filename.kl" ::  — inline the referenced file's contents

    def _resolve_use_path(self, node: UseStmt) -> str:
        """Resolve a use statement's filepath to an absolute path."""
        fpath = node.filepath
        if self._root_dir and not os.path.isabs(fpath):
            fpath = os.path.normpath(os.path.join(self._root_dir, fpath))
        return os.path.normcase(os.path.abspath(fpath))

    def _gen_use_prepare(self, node: UseStmt):
        """Pre-process a UseStmt: resolve path, parse file, store body in node.body.
        Called BEFORE main() so func/class decls end up at the right scope."""
        if node.body:
            return  # already prepared

        fpath = self._resolve_use_path(node)

        if fpath in self._included:
            node.body = []
            return
        if not os.path.exists(fpath):
            self._error(f"File not found: '{node.filepath}' (resolved: {fpath})", node)
            return

        self._included.add(fpath)

        try:
            with open(fpath, "r", encoding="utf-8") as f:
                src = f.read()
        except Exception as e:
            self._error(f"Cannot read '{node.filepath}': {e}", node)
            return

        filen = os.path.basename(fpath)
        try:
            lexer = Lexer(src, filen)
            tokens = lexer.tokenise()
            parser = Parser(tokens, filen)
            included_ast = parser.parse()
        except Exception as e:
            self._emit(f"/* Error parsing '{node.filepath}': {e} */")
            return

        node.body = included_ast.body

        # Recursively prepare any nested UseStmt nodes in the included file
        for n in included_ast.body:
            if isinstance(n, UseStmt):
                self._gen_use_prepare(n)

    def _gen_use(self, node: UseStmt):
        """Emit the statements from an included file at the current position.
        Called inside main() — only emits non-func/class statements."""
        if not node.body:
            return
        self._emit(f"/* --- begin '{node.filepath}' --- */")
        for n in node.body:
            if not isinstance(n, (FuncDecl, ClassDecl)):
                self._gen_stmt(n)
        self._emit(f"/* --- end '{node.filepath}' --- */")

    # ── Expression codegen ────────────────────────────────────────────────

    def _gen_expr(self, node) -> str:
        if isinstance(node, IntLiteral):
            return f"kv_int({node.value}LL)"

        if isinstance(node, FltLiteral):
            return f"kv_flt({node.value})"

        if isinstance(node, StrLiteral):
            escaped = node.value.replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n").replace("\t", "\\t")
            return f'kv_str("{escaped}")'

        if isinstance(node, BoolLiteral):
            return f"kv_bool({'true' if node.value else 'false'})"

        if isinstance(node, VoidLiteral):
            return "kv_void()"

        if isinstance(node, Identifier):
            if node.name == "PI": return "K_PI"
            if node.name == "E":  return "K_E"
            return node.name

        if isinstance(node, MemberAccess):
            obj = self._gen_expr(node.obj)
            return f'kdict_get({obj}->dict, "{node.member}")'

        if isinstance(node, IndexAccess):
            obj = self._gen_expr(node.obj)
            idx = self._gen_expr(node.index)
            if node.end is None:
                return f"k_Lget({obj}, {idx})"
            # Slice — return sub-list
            end  = self._gen_expr(node.end)
            return f"k_Lget({obj}, {idx})"  # simplified v1

        if isinstance(node, BinOp):
            return self._gen_binop(node)

        if isinstance(node, UnaryOp):
            operand = self._gen_expr(node.operand)
            if node.op == "-":
                return f"kv_flt(-_knum({operand}))"
            if node.op == "!&":
                return f"k_not({operand})"
            return operand

        if isinstance(node, FuncCall):
            args_c = ", ".join(self._gen_expr(a) for a in node.args)
            if node.obj:
                return self._gen_method_call(node)
            return f"kf_{node.name}({args_c})"

        if isinstance(node, BuiltinCall):
            return self._gen_builtin(node)

        if isinstance(node, AskExpr):
            prompt = self._gen_expr(node.prompt)
            return f"k_ask({prompt}->s)"

        if isinstance(node, DictLiteral):
            return self._gen_inline_dict(node)

        if isinstance(node, ListLiteral):
            # inline list — generate temp
            return self._gen_inline_list(node)

        if isinstance(node, TupleLiteral):
            # Treat tuple as list (immutable by convention)
            return self._gen_inline_list(node)

        if isinstance(node, TemplateString):
            return self._gen_template_string(node)

        self._error(f"Cannot generate expression for {type(node).__name__}", node)

    def _gen_binop(self, node: BinOp) -> str:
        l = self._gen_expr(node.left)
        r = self._gen_expr(node.right)
        ops = {
            "+":   f"k_add({l},{r})",
            "-":   f"k_sub({l},{r})",
            "*":   f"k_mul({l},{r})",
            "/":   f"k_div({l},{r})",
            "//":  f"k_idiv({l},{r})",
            "///": f"k_mod({l},{r})",
            "^":   f"k_pow({l},{r})",
            "%":   f"k_pct({l},{r})",
            "++":  f"k_concat({l},{r})",
            "==":  f"k_eq({l},{r})",
            "=\\": f"k_neq({l},{r})",
            "<":   f"k_lt({l},{r})",
            ">":   f"k_gt({l},{r})",
            "=<":  f"k_lte({l},{r})",
            "=>":  f"k_gte({l},{r})",
            "&&":  f"k_and({l},{r})",
            "&+":  f"k_or({l},{r})",
        }
        result = ops.get(node.op)
        if result is None:
            self._error(f"Unknown operator: {node.op}", node)
        return result

    def _gen_builtin(self, node: BuiltinCall) -> str:
        fn = node.func
        if fn == "testcheck":
            args_c = ", ".join(self._gen_expr(a) for a in node.args)
            return f"k_testcheck({args_c}, \"<expr>\")"

        c  = BUILTIN_MAP.get(fn)
        if c:
            args_c = ", ".join(self._gen_expr(a) for a in node.args)
            if fn in VOID_BUILTINS:
                return f"({c}({args_c}), kv_void())"
            return f"{c}({args_c})"

        # Special cases
        if fn == "range":
            return "kv_void()"  # range is handled in for-range directly

        self._error(f"Unknown built-in: '{fn}'", node)

    def _gen_inline_dict(self, node: DictLiteral) -> str:
        # Emit as a compound statement expression using GCC extension
        parts = []
        for key, val_node in node.pairs:
            kv = self._gen_expr(key)
            vv = self._gen_expr(val_node)
            if isinstance(key, StrLiteral):
                parts.append(f'kdict_set(_tmp_d->dict, {kv}->s, {vv})')
            else:
                parts.append(f'kdict_set(_tmp_d->dict, k_S({kv})->s, {vv})')
        inner = "; ".join(parts)
        return f"({{ KVal* _tmp_d = kv_dict(); {inner}; _tmp_d; }})"

    def _gen_inline_list(self, node: ListLiteral) -> str:
        # Build inline list expression using a comma expression trick
        # We emit a helper block — not clean but works in statement context
        # For inline, we create a temporary
        parts = [f"k_Ladd(_tmp_l, {self._gen_expr(e)})" for e in node.elements]
        return "(({KVal* _tmp_l = kv_list(); " + "; ".join(parts) + "; _tmp_l;}))"

    def _gen_template_string(self, node: TemplateString) -> str:
        """Generate code for template strings like "Hello {name}!"""
        if not node.parts:
            return 'kv_str("")'
        if len(node.parts) == 1 and isinstance(node.parts[0], StrLiteral):
            # No template expressions — treat as normal string
            return self._gen_expr(node.parts[0])
        
        # Build concatenation: k_concat(k_concat(kv_str("Hello "), name), kv_str("!"))
        result = None
        for part in node.parts:
            part_code = self._gen_expr(part)
            if result is None:
                result = part_code
            else:
                result = f"k_concat({result}, {part_code})"
        return result if result else 'kv_str("")'

    # ── Method call helper ─────────────────────────────────────────────────

    def _gen_method_call(self, node: FuncCall) -> str:
        """Generate a method call like obj.method(args)"""
        obj_code = self._gen_expr(node.obj)
        args_c = ", ".join(self._gen_expr(a) for a in node.args)
        # Look up the class type from the object variable name
        cls_name = None
        if isinstance(node.obj, Identifier) and node.obj.name in self._class_types:
            cls_name = self._class_types[node.obj.name]
        
        if cls_name:
            if args_c:
                return f"kmethod_{cls_name}_{node.name}({obj_code}, {args_c})"
            return f"kmethod_{cls_name}_{node.name}({obj_code})"
        else:
            # Dynamic dispatch: look up method in object's dict at runtime
            # For now, emit a wrapped dynamic call placeholder
            return f"k_method_call({obj_code}, kv_str('{node.name}'), {args_c})"
