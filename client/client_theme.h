#ifndef SMART_HOME_CLIENT_THEME_H
#define SMART_HOME_CLIENT_THEME_H

#include <QString>

namespace client_theme {

inline QString light() {
    return QStringLiteral(R"QSS(
QMainWindow, QWidget#centralPanel { background:#F3F5F7; color:#202833; }
QWidget { font-family:"Microsoft YaHei UI"; font-size:17px; }
QDialog { background:#F3F5F7; color:#202833; }
QDialog QLabel, QMessageBox QLabel, QInputDialog QLabel { color:#202833; }
QMenuBar { background:#FFFFFF; color:#35414D; border-bottom:1px solid #E1E6EB; padding:5px 10px; }
QMenuBar::item { background:transparent; border-radius:5px; padding:8px 12px; }
QMenuBar::item:selected { background:#E9F0EF; color:#285D58; }
QMenu { background:#FFFFFF; color:#35414D; border:1px solid #DDE3E8; border-radius:8px; padding:6px; }
QMenu::item { border-radius:5px; padding:9px 32px 9px 14px; }
QMenu::item:selected { background:#E6EFEE; color:#285D58; }
QMenu::separator { height:1px; background:#E5E9ED; margin:5px 8px; }
QToolBar { background:#FFFFFF; border:none; border-bottom:1px solid #E1E6EB; spacing:6px; padding:8px 12px; }
QToolBar::separator { width:1px; background:#DDE3E8; margin:5px 7px; }
QToolButton { background:transparent; color:#44515D; border:1px solid transparent; border-radius:7px; padding:8px 12px; font-weight:600; }
QToolButton:hover { background:#EDF3F2; border-color:#D5E1DF; }
QToolButton:pressed, QToolButton:checked { background:#D9E9E7; color:#285D58; border-color:#A9C6C2; }
QStatusBar { background:#FFFFFF; color:#65717D; border-top:1px solid #E1E6EB; }
QStatusBar::item { border:none; }
QDockWidget { color:#35414D; font-weight:600; }
QDockWidget::title { background:#FFFFFF; border-bottom:1px solid #E1E6EB; padding:8px 12px; text-align:left; }
QFrame[card="true"] { background:#FFFFFF; border:1px solid #E1E6EB; border-radius:12px; }
QLabel { background:transparent; border:none; }
QLabel[role="appTitle"] { color:#17212B; font-size:25px; font-weight:700; }
QLabel[role="title"] { color:#26313D; font-size:20px; font-weight:700; }
QLabel[role="subtitle"] { color:#74808D; font-size:17px; }
QLabel[role="field"] { color:#606C78; font-size:17px; font-weight:600; }
QLabel[role="infoValue"] { color:#1F2937; font-size:17px; font-weight:600; }
QLabel[role="infoValue"][tone="neutral"] { color:#64748B; }
QLabel[role="infoValue"][tone="success"] { color:#059669; }
QLabel[role="infoValue"][tone="warning"] { color:#D97706; }
QLabel[role="infoValue"][tone="error"] { color:#DC2626; }
QLabel[role="badge"] { border-radius:13px; padding:6px 12px; font-size:17px; font-weight:600; }
QLabel[role="badge"][tone="neutral"] { background:#EDF1F4; color:#5F6B77; }
QLabel[role="badge"][tone="success"] { background:#E4F2ED; color:#28745A; }
QLabel[role="badge"][tone="warning"] { background:#F8EEDA; color:#8C6420; }
QLabel[role="badge"][tone="info"] { background:#E7EFF4; color:#3D697C; }
QLineEdit, QSpinBox, QPlainTextEdit, QTreeWidget {
    background:#FBFCFD; color:#26313D; border:1px solid #D8DEE5; border-radius:8px;
    selection-background-color:#5F8E8A; selection-color:#FFFFFF;
}
QLineEdit, QSpinBox { min-height:32px; padding:7px 10px; }
QLineEdit:hover, QSpinBox:hover { border-color:#B8C3CC; background:#FFFFFF; }
QLineEdit:focus, QSpinBox:focus, QPlainTextEdit:focus, QTreeWidget:focus { border:1px solid #588783; background:#FFFFFF; }
QLineEdit:disabled, QSpinBox:disabled { background:#F0F2F4; color:#9AA3AC; }
QSpinBox::up-button, QSpinBox::down-button { width:24px; border:none; background:transparent; }
QCheckBox { color:#52606C; spacing:7px; background:transparent; }
QCheckBox::indicator { width:20px; height:20px; border:1px solid #B9C4CC; border-radius:4px; background:#FFFFFF; }
QCheckBox::indicator:hover { border-color:#6F9995; }
QCheckBox::indicator:checked { background:#467D78; border-color:#467D78; image:none; }
QTreeWidget { padding:4px; outline:none; alternate-background-color:#F7F9FA; }
QTreeWidget::item { min-height:38px; border-radius:6px; padding:3px 7px; }
QTreeWidget::item:hover { background:#EDF3F2; }
QTreeWidget::item:selected { background:#DDEBE9; color:#285D58; }
QHeaderView::section { background:#F6F8F9; color:#64717D; padding:10px 11px; border:none; border-bottom:1px solid #E1E6EB; font-size:17px; font-weight:600; }
QPushButton { min-height:32px; background:#F7F9FA; color:#35414D; border:1px solid #D5DCE2; border-radius:8px; padding:8px 14px; font-weight:600; }
QPushButton:hover { background:#EDF2F3; border-color:#AEBBC2; }
QPushButton:pressed { background:#DCE5E6; border-color:#789794; padding-top:10px; padding-bottom:6px; }
QPushButton:checked { background:#D9EAE8; color:#245B56; border-color:#6E9995; }
QPushButton:disabled { background:#F1F3F5; color:#A3ABB3; border-color:#E0E4E8; }
QPushButton[role="primary"] { background:#3F7773; color:#FFFFFF; border-color:#3F7773; }
QPushButton[role="primary"]:hover { background:#356B67; border-color:#356B67; }
QPushButton[role="primary"]:pressed, QPushButton[role="primary"]:checked { background:#285C58; border-color:#285C58; }
QPushButton[role="danger"] { background:#FBF4F4; color:#A64D55; border-color:#E7C9CC; }
QPushButton[role="danger"]:hover { background:#F6E7E8; border-color:#D79FA4; }
QPushButton[role="danger"]:pressed, QPushButton[role="danger"]:checked { background:#B75A63; color:#FFFFFF; border-color:#A94C55; }
QPushButton[role="ptz"] { font-size:17px; padding:7px; }
QPushButton[role="layout"] { min-width:30px; min-height:30px; padding:6px 10px; font-size:17px; }
QPushButton#themeButton { background:#26313D; color:#F7F9FA; border-color:#26313D; }
QPushButton#themeButton:hover { background:#17212B; }
QTabWidget::pane { background:#FFFFFF; border:1px solid #E1E6EB; border-radius:10px; top:-1px; }
QTabBar::tab { background:#EDF1F4; color:#68747F; border:1px solid #E1E6EB; padding:7px 14px; margin-right:3px; border-top-left-radius:8px; border-top-right-radius:8px; }
QTabBar::tab:hover { background:#E7EEED; }
QTabBar::tab:selected { background:#FFFFFF; color:#315F5B; border-bottom-color:#FFFFFF; font-weight:600; }
QStackedWidget#videoSurface { background:#0D141A; border-radius:10px; }
QLabel#liveView { background:#0D141A; border-radius:10px; color:#8E9CA8; }
QLabel#liveView[previewState="idle"] { background:#E8EDF0; color:#60717E; font-size:17px; }
QLabel#liveView[previewState="active"] { background:#080C10; color:#8E9CA8; }
QPlainTextEdit#logView { font-family:"Consolas", "Microsoft YaHei UI"; font-size:17px; padding:10px; }
QSplitter::handle { background:transparent; width:8px; height:8px; }
QSplitter::handle:hover { background:#DCE4E7; border-radius:3px; }
QScrollBar:vertical { background:transparent; width:10px; margin:2px; }
QScrollBar:horizontal { background:transparent; height:10px; margin:2px; }
QScrollBar::handle { background:#C3CCD3; border-radius:4px; min-height:28px; min-width:28px; }
QScrollBar::handle:hover { background:#AAB6BF; }
QScrollBar::add-line, QScrollBar::sub-line { width:0; height:0; }
QToolTip { background:#26313D; color:#FFFFFF; border:none; padding:6px; }
)QSS");
}

inline QString dark() {
    return QStringLiteral(R"QSS(
QMainWindow, QWidget#centralPanel { background:#0E1319; color:#E6EBF0; }
QWidget { font-family:"Microsoft YaHei UI"; font-size:17px; }
QDialog { background:#0E1319; color:#E6EBF0; }
QDialog QLabel, QMessageBox QLabel, QInputDialog QLabel { color:#E6EDF3; }
QMenuBar { background:#171E26; color:#D5DDE4; border-bottom:1px solid #27323D; padding:5px 10px; }
QMenuBar::item { background:transparent; border-radius:5px; padding:8px 12px; }
QMenuBar::item:selected { background:#24332F; color:#B6D8D4; }
QMenu { background:#171E26; color:#D5DDE4; border:1px solid #34404B; border-radius:8px; padding:6px; }
QMenu::item { border-radius:5px; padding:9px 32px 9px 14px; }
QMenu::item:selected { background:#29413E; color:#D9EFEC; }
QMenu::separator { height:1px; background:#303B46; margin:5px 8px; }
QToolBar { background:#171E26; border:none; border-bottom:1px solid #27323D; spacing:6px; padding:8px 12px; }
QToolBar::separator { width:1px; background:#34404B; margin:5px 7px; }
QToolButton { background:transparent; color:#C7D0D8; border:1px solid transparent; border-radius:7px; padding:8px 12px; font-weight:600; }
QToolButton:hover { background:#23302F; border-color:#354A47; }
QToolButton:pressed, QToolButton:checked { background:#294642; color:#DDF2EE; border-color:#4F7773; }
QStatusBar { background:#171E26; color:#99A5B0; border-top:1px solid #27323D; }
QStatusBar::item { border:none; }
QDockWidget { color:#D5DDE4; font-weight:600; }
QDockWidget::title { background:#171E26; border-bottom:1px solid #27323D; padding:8px 12px; text-align:left; }
QFrame[card="true"] { background:#171E26; border:1px solid #27323D; border-radius:12px; }
QLabel { background:transparent; border:none; }
QLabel[role="appTitle"] { color:#F0F4F7; font-size:25px; font-weight:700; }
QLabel[role="title"] { color:#E6EBF0; font-size:20px; font-weight:700; }
QLabel[role="subtitle"] { color:#8C99A6; font-size:17px; }
QLabel[role="field"] { color:#A4AFBA; font-size:17px; font-weight:600; }
QLabel[role="infoValue"] { color:#E6EDF3; font-size:17px; font-weight:600; }
QLabel[role="infoValue"][tone="neutral"] { color:#94A3B8; }
QLabel[role="infoValue"][tone="success"] { color:#34D399; }
QLabel[role="infoValue"][tone="warning"] { color:#F59E0B; }
QLabel[role="infoValue"][tone="error"] { color:#F87171; }
QLabel[role="badge"] { border-radius:13px; padding:6px 12px; font-size:17px; font-weight:600; }
QLabel[role="badge"][tone="neutral"] { background:#27313B; color:#AEB8C2; }
QLabel[role="badge"][tone="success"] { background:#193B31; color:#82C9AE; }
QLabel[role="badge"][tone="warning"] { background:#40331E; color:#E4BC70; }
QLabel[role="badge"][tone="info"] { background:#213743; color:#88B9CE; }
QLineEdit, QSpinBox, QPlainTextEdit, QTreeWidget {
    background:#11171E; color:#E2E8ED; border:1px solid #35414E; border-radius:8px;
    selection-background-color:#598985; selection-color:#FFFFFF;
}
QLineEdit, QSpinBox { min-height:32px; padding:7px 10px; }
QLineEdit:hover, QSpinBox:hover { border-color:#4D5D6B; background:#141B23; }
QLineEdit:focus, QSpinBox:focus, QPlainTextEdit:focus, QTreeWidget:focus { border:1px solid #72A39F; }
QLineEdit:disabled, QSpinBox:disabled { background:#151B22; color:#66727E; }
QSpinBox::up-button, QSpinBox::down-button { width:24px; border:none; background:transparent; }
QCheckBox { color:#AAB5BF; spacing:7px; background:transparent; }
QCheckBox::indicator { width:20px; height:20px; border:1px solid #4B5966; border-radius:4px; background:#11171E; }
QCheckBox::indicator:hover { border-color:#77A5A1; }
QCheckBox::indicator:checked { background:#568B86; border-color:#568B86; image:none; }
QTreeWidget { padding:4px; outline:none; alternate-background-color:#141B22; }
QTreeWidget::item { min-height:38px; border-radius:6px; padding:3px 7px; }
QTreeWidget::item:hover { background:#202C33; }
QTreeWidget::item:selected { background:#294642; color:#DDF2EE; }
QHeaderView::section { background:#1C252E; color:#98A5B1; padding:10px 11px; border:none; border-bottom:1px solid #2C3742; font-size:17px; font-weight:600; }
QPushButton { min-height:32px; background:#202933; color:#DCE3E9; border:1px solid #394653; border-radius:8px; padding:8px 14px; font-weight:600; }
QPushButton:hover { background:#28343F; border-color:#52616F; }
QPushButton:pressed { background:#182A2B; border-color:#6F9B97; padding-top:10px; padding-bottom:6px; }
QPushButton:checked { background:#294945; color:#DDF3EF; border-color:#6B9D98; }
QPushButton:disabled { background:#181E25; color:#65717C; border-color:#28313A; }
QPushButton[role="primary"] { background:#4E8580; color:#FFFFFF; border-color:#4E8580; }
QPushButton[role="primary"]:hover { background:#5A928D; border-color:#5A928D; }
QPushButton[role="primary"]:pressed, QPushButton[role="primary"]:checked { background:#326965; border-color:#6BA09B; }
QPushButton[role="danger"] { background:#342428; color:#E7A2A8; border-color:#60383E; }
QPushButton[role="danger"]:hover { background:#432B30; border-color:#895159; }
QPushButton[role="danger"]:pressed, QPushButton[role="danger"]:checked { background:#934B54; color:#FFFFFF; border-color:#B66A72; }
QPushButton[role="ptz"] { font-size:17px; padding:7px; }
QPushButton[role="layout"] { min-width:30px; min-height:30px; padding:6px 10px; font-size:17px; }
QPushButton#themeButton { background:#E5EBEF; color:#17212B; border-color:#E5EBEF; }
QPushButton#themeButton:hover { background:#FFFFFF; }
QTabWidget::pane { background:#171E26; border:1px solid #27323D; border-radius:10px; top:-1px; }
QTabBar::tab { background:#11171E; color:#8996A2; border:1px solid #27323D; padding:7px 14px; margin-right:3px; border-top-left-radius:8px; border-top-right-radius:8px; }
QTabBar::tab:hover { background:#202A34; }
QTabBar::tab:selected { background:#171E26; color:#A9D1CD; border-bottom-color:#171E26; font-weight:600; }
QStackedWidget#videoSurface { background:#070A0E; border-radius:10px; }
QLabel#liveView { background:#070A0E; border-radius:10px; color:#82909C; }
QLabel#liveView[previewState="idle"] { background:#111A22; color:#8FA0AD; font-size:17px; }
QLabel#liveView[previewState="active"] { background:#05080B; color:#82909C; }
QPlainTextEdit#logView { font-family:"Consolas", "Microsoft YaHei UI"; font-size:17px; padding:10px; }
QSplitter::handle { background:transparent; width:8px; height:8px; }
QSplitter::handle:hover { background:#2A3740; border-radius:3px; }
QScrollBar:vertical { background:transparent; width:10px; margin:2px; }
QScrollBar:horizontal { background:transparent; height:10px; margin:2px; }
QScrollBar::handle { background:#3B4854; border-radius:4px; min-height:28px; min-width:28px; }
QScrollBar::handle:hover { background:#50606E; }
QScrollBar::add-line, QScrollBar::sub-line { width:0; height:0; }
QToolTip { background:#E5EBEF; color:#17212B; border:none; padding:6px; }
)QSS");
}

}  // namespace client_theme

#endif
