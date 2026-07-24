#ifndef SMART_HOME_CLIENT_THEME_H
#define SMART_HOME_CLIENT_THEME_H

#include <QString>

namespace client_theme {

inline QString light() {
    return QStringLiteral(R"QSS(
QMainWindow, QWidget#centralPanel { background:#F3F6F9; color:#1F2937; }
QWidget { font-family:"Microsoft YaHei UI"; font-size:17px; }
QDialog { background:#F3F6F9; color:#1F2937; }
QDialog QLabel, QMessageBox QLabel, QInputDialog QLabel { color:#1F2937; }
QMenuBar { background:#FFFFFF; color:#344054; border-bottom:1px solid #E5EAF0; padding:5px 10px; }
QMenuBar::item { background:transparent; border-radius:5px; padding:8px 12px; }
QMenuBar::item:selected { background:#E8F2FC; color:#1269B0; }
QMenu { background:#FFFFFF; color:#344054; border:1px solid #D7E0E8; border-radius:8px; padding:6px; }
QMenu::item { border-radius:5px; padding:9px 32px 9px 14px; }
QMenu::item:selected { background:#E8F2FC; color:#1269B0; }
QMenu::separator { height:1px; background:#E5EAF0; margin:5px 8px; }
QToolBar { background:#FFFFFF; border:none; border-bottom:1px solid #E5EAF0; spacing:6px; padding:8px 12px; }
QToolBar::separator { width:1px; background:#D7E0E8; margin:5px 7px; }
QToolButton { background:transparent; color:#344054; border:1px solid transparent; border-radius:7px; padding:8px 12px; font-weight:600; }
QToolButton:hover { background:#E8F2FC; border-color:#DCEEFF; color:#1269B0; }
QToolButton:pressed, QToolButton:checked { background:#DCEEFF; color:#1269B0; border-color:#7CB8EB; }
QToolButton[role="primary"] { background:#1683E2; color:#FFFFFF; border-color:#1683E2; }
QToolButton[role="primary"]:hover { background:#0F74C9; color:#FFFFFF; border-color:#0F74C9; }
QToolButton[role="primary"]:pressed, QToolButton[role="primary"]:checked { background:#0C63AD; color:#FFFFFF; border-color:#0C63AD; }
QStatusBar { background:#FFFFFF; color:#667085; border-top:1px solid #DDE4EA; padding:3px 6px; }
QStatusBar::item { border:none; }
QDockWidget { color:#344054; font-weight:600; }
QDockWidget::title { background:#FFFFFF; border-bottom:1px solid #E5EAF0; padding:8px 12px; text-align:left; }
QFrame[card="true"] { background:#FFFFFF; border:1px solid #D7E0E8; border-radius:10px; }
QLabel { background:transparent; border:none; }
QLabel[role="appTitle"] { color:#1F2937; font-size:25px; font-weight:700; }
QLabel[role="title"] { color:#1F2937; font-size:20px; font-weight:700; }
QLabel[role="subtitle"] { color:#667085; font-size:17px; }
QLabel[role="field"] { color:#344054; font-size:17px; font-weight:600; }
QLabel[role="infoValue"] { color:#1F2937; font-size:17px; font-weight:600; }
QLabel[role="infoValue"][tone="neutral"] { color:#64748B; }
QLabel[role="infoValue"][tone="success"] { color:#16A34A; }
QLabel[role="infoValue"][tone="warning"] { color:#D97706; }
QLabel[role="infoValue"][tone="error"] { color:#DC2626; }
QLabel[role="badge"] { border-radius:13px; padding:6px 12px; font-size:17px; font-weight:600; border:1px solid transparent; }
QLabel[role="badge"][tone="neutral"] { background:#F1F5F9; color:#64748B; border-color:#E2E8F0; }
QLabel[role="badge"][tone="success"] { background:#E8F7EE; color:#15803D; border-color:#B7E4C7; }
QLabel[role="badge"][tone="warning"] { background:#FFF7E8; color:#D97706; border-color:#F7D9A4; }
QLabel[role="badge"][tone="info"] { background:#E6F7F9; color:#087B8F; border-color:#B8E3E8; }
QLabel[role="emptyIcon"] { color:#1683E2; font-size:25px; font-weight:700; }
QLabel[role="emptyTitle"] { color:#344054; font-size:20px; font-weight:700; }
QLabel[role="emptyHint"] { color:#667085; font-size:17px; }
QLabel[role="sliderEdge"] { color:#667085; font-size:17px; }
QLabel[role="sliderValue"] { background:#E4F2FF; color:#1269B0; border:1px solid #B8DDFC; border-radius:7px; padding:5px 8px; font-size:17px; font-weight:700; }
QWidget#deviceEmptyState { background:#EEF3F7; border:1px solid #E1E8EF; border-radius:8px; }
QStackedWidget#deviceContentStack { background:transparent; border:none; }
QLineEdit, QSpinBox, QPlainTextEdit, QTreeWidget {
    background:#FFFFFF; color:#344054; border:1px solid #CBD5E1; border-radius:7px;
    selection-background-color:#1683E2; selection-color:#FFFFFF;
}
QLineEdit, QSpinBox { min-height:32px; padding:7px 10px; }
QLineEdit:hover, QSpinBox:hover { border-color:#94A3B8; background:#FFFFFF; }
QLineEdit:focus, QSpinBox:focus, QPlainTextEdit:focus, QTreeWidget:focus { border:1px solid #1683E2; background:#FFFFFF; }
QLineEdit:disabled, QSpinBox:disabled { background:#F2F4F7; color:#98A2B3; }
QSpinBox::up-button, QSpinBox::down-button { width:24px; border:none; background:transparent; }
QCheckBox { color:#344054; spacing:7px; background:transparent; }
QCheckBox::indicator { width:20px; height:20px; border:1px solid #CBD5E1; border-radius:4px; background:#FFFFFF; }
QCheckBox::indicator:hover { border-color:#7CB8EB; }
QCheckBox::indicator:checked { background:#1683E2; border-color:#1683E2; image:none; }
QTreeWidget { padding:4px; outline:none; alternate-background-color:#F8FAFC; selection-background-color:#E4F2FF; selection-color:#1269B0; }
QTreeWidget::item { min-height:38px; border-radius:5px; padding:3px 7px; border-left:3px solid transparent; }
QTreeWidget::item:hover { background:#E8F2FC; }
QTreeWidget::item:selected { background:#E4F2FF; color:#1269B0; border-left:3px solid #1683E2; }
QHeaderView::section { background:#EEF3F7; color:#667085; padding:10px 11px; border:none; border-bottom:1px solid #D7E0E8; font-size:17px; font-weight:600; }
QPushButton { min-height:32px; background:#F7F9FC; color:#344054; border:1px solid #CCD6E0; border-radius:8px; padding:8px 14px; font-weight:600; }
QPushButton:hover { background:#E8F2FC; color:#1269B0; border-color:#7CB8EB; }
QPushButton:pressed { background:#DCEEFF; color:#1269B0; border-color:#1683E2; padding-top:10px; padding-bottom:6px; }
QPushButton:checked { background:#DCEEFF; color:#1269B0; border-color:#1683E2; }
QPushButton:disabled { background:#F2F4F7; color:#98A2B3; border-color:#E4E7EC; }
QPushButton[role="primary"] { background:#1683E2; color:#FFFFFF; border-color:#1683E2; }
QPushButton[role="primary"]:hover { background:#0F74C9; color:#FFFFFF; border-color:#0F74C9; }
QPushButton[role="primary"]:pressed, QPushButton[role="primary"]:checked { background:#0C63AD; color:#FFFFFF; border-color:#0C63AD; }
QPushButton[role="softPrimary"] { background:#E4F2FF; color:#1269B0; border-color:#B8DDFC; }
QPushButton[role="softPrimary"]:hover { background:#DCEEFF; border-color:#7CB8EB; }
QPushButton[role="danger"] { background:#FFF1F1; color:#C24141; border-color:#F3C0C0; }
QPushButton[role="danger"]:hover { background:#FDE7E7; border-color:#E89A9A; }
QPushButton[role="danger"]:pressed, QPushButton[role="danger"]:checked { background:#DC6262; color:#FFFFFF; border-color:#C94E4E; }
QPushButton[tone="recording"] { background:#DC2626; color:#FFFFFF; border-color:#B91C1C; }
QPushButton[tone="recording"]:hover { background:#B91C1C; color:#FFFFFF; border-color:#991B1B; }
QPushButton[role="ptz"] { font-size:17px; padding:7px; }
QPushButton[role="ptz"][tone="stop"] { background:#FFF1F1; color:#C24141; border-color:#F3C0C0; }
QPushButton[role="ptz"][tone="stop"]:hover { background:#FDE7E7; border-color:#E89A9A; }
QPushButton[role="ptz"][tone="stop"]:pressed { background:#F8CCCC; color:#A52E2E; border-color:#D96F6F; }
QPushButton[role="layout"] { min-width:30px; min-height:30px; padding:6px 10px; font-size:17px; }
QPushButton#themeButton { background:#26313D; color:#F7F9FA; border-color:#26313D; }
QPushButton#themeButton:hover { background:#17212B; }
QTabWidget::pane { background:#FFFFFF; border:1px solid #E1E6EB; border-radius:10px; top:-1px; }
QTabBar::tab { background:#EDF1F4; color:#68747F; border:1px solid #E1E6EB; padding:7px 14px; margin-right:3px; border-top-left-radius:8px; border-top-right-radius:8px; }
QTabBar::tab:hover { background:#E7EEED; }
QTabBar::tab:selected { background:#FFFFFF; color:#315F5B; border-bottom-color:#FFFFFF; font-weight:600; }
QFrame#videoFrame { background:#E8EDF3; border:none; border-radius:9px; }
QStackedWidget#videoSurface { background:#080D14; border-radius:7px; }
QLabel#liveView { background:#080D14; border-radius:7px; color:#CBD5E1; }
QLabel#liveView[previewState="idle"] { background:#080D14; color:#CBD5E1; font-size:17px; }
QLabel#liveView[previewState="active"] { background:#080D14; color:#CBD5E1; }
QWidget#videoGrid { background:#080D14; }
QLabel#videoGridEmptyState { background:#080D14; color:#CBD5E1; font-size:17px; }
QFrame#videoTile { background:#101827; border:2px solid #334155; border-radius:8px; }
QFrame#videoTile[active="true"] { border:2px solid #38A7F2; }
QFrame#videoTileHeader, QFrame#videoTileFooter { background:#162131; border:none; }
QLabel#videoTileSurface { background:#05080C; color:#B9C6D5; border:none; }
QLabel[role="videoTitle"] { color:#F4F7FB; font-weight:700; }
QLabel[role="videoStatus"][tone="neutral"] { color:#AAB7C6; }
QLabel[role="videoStatus"][tone="info"] { color:#7DC8F5; }
QLabel[role="videoStatus"][tone="success"] { color:#72D6A7; }
QLabel[role="videoStatus"][tone="warning"] { color:#F2B86B; }
QLabel[role="videoHint"] { color:#8D9AAC; font-size:15px; }
QSlider::groove:horizontal { height:6px; background:#DCE6EF; border-radius:3px; }
QSlider::sub-page:horizontal { background:#1683E2; border-radius:3px; }
QSlider::add-page:horizontal { background:#DCE6EF; border-radius:3px; }
QSlider::handle:horizontal { width:18px; height:18px; margin:-6px 0; background:#FFFFFF; border:2px solid #1683E2; border-radius:9px; }
QSlider::handle:horizontal:hover { background:#E4F2FF; border-color:#0F74C9; }
QPlainTextEdit#logView { font-family:"Consolas", "Microsoft YaHei UI"; font-size:17px; padding:10px; }
QSplitter::handle { background:transparent; width:8px; height:8px; }
QSplitter::handle:hover { background:#D7E0E8; border-radius:3px; }
QScrollBar:vertical { background:transparent; width:10px; margin:2px; }
QScrollBar:horizontal { background:transparent; height:10px; margin:2px; }
QScrollBar::handle { background:#C7D2DC; border-radius:4px; min-height:28px; min-width:28px; }
QScrollBar::handle:hover { background:#A8B7C4; }
QScrollBar::add-line, QScrollBar::sub-line { width:0; height:0; }
QToolTip { background:#26313D; color:#FFFFFF; border:none; padding:6px; }
)QSS");
}

inline QString dark() {
    return QStringLiteral(R"QSS(
QMainWindow, QWidget#centralPanel {
    background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #23122C,stop:0.50 #1D1831,stop:1 #151F37);
    color:#F4F6FA;
}
QWidget { font-family:"Microsoft YaHei UI"; font-size:17px; color:#F4F6FA; }
QDialog { background:#171829; color:#F4F6FA; }
QDialog QLabel, QMessageBox QLabel, QInputDialog QLabel { color:#F4F6FA; }
QMenuBar { background:#1B1A2B; color:#E9EDF4; border-bottom:1px solid #34354E; padding:5px 10px; }
QMenuBar::item { background:transparent; border-radius:5px; padding:8px 12px; }
QMenuBar::item:selected { background:#343451; color:#FFFFFF; }
QMenu { background:#1B1A2B; color:#E9EDF4; border:1px solid #41435E; border-radius:8px; padding:6px; }
QMenu::item { border-radius:5px; padding:9px 32px 9px 14px; }
QMenu::item:selected { background:#354564; color:#FFFFFF; }
QMenu::separator { height:1px; background:#383A52; margin:5px 8px; }
QToolBar { background:#1B1A2B; border:none; border-bottom:1px solid #34354E; spacing:6px; padding:8px 12px; }
QToolBar::separator { width:1px; background:#41435E; margin:5px 7px; }
QToolButton { background:transparent; color:#E5EAF1; border:1px solid transparent; border-radius:7px; padding:8px 12px; font-weight:600; }
QToolButton:hover { background:#2A3048; border-color:#46516E; color:#FFFFFF; }
QToolButton:pressed, QToolButton:checked { background:#354564; color:#FFFFFF; border-color:#66799E; }
QToolButton[role="primary"] { background:#356D78; color:#FFFFFF; border-color:#4D8792; }
QToolButton[role="primary"]:hover { background:#3F7D88; color:#FFFFFF; border-color:#63A0AA; }
QToolButton[role="primary"]:pressed, QToolButton[role="primary"]:checked { background:#2E616C; color:#FFFFFF; border-color:#73AAB2; }
QStatusBar { background:#1B1A2B; color:#B8C2D0; border-top:1px solid #34354E; }
QStatusBar::item { border:none; }
QDockWidget { color:#E9EDF4; font-weight:600; }
QDockWidget::title { background:#1B1A2B; border-bottom:1px solid #34354E; padding:8px 12px; text-align:left; }
QFrame[card="true"] { background:#1B1F31; border:1px solid #393D55; border-radius:12px; }
QLabel { background:transparent; border:none; color:#F4F6FA; }
QLabel[role="appTitle"] { color:#FFFFFF; font-size:25px; font-weight:700; }
QLabel[role="title"] { color:#F4F6FA; font-size:20px; font-weight:700; }
QLabel[role="subtitle"] { color:#B8C2D0; font-size:17px; }
QLabel[role="field"] { color:#CDD5E1; font-size:17px; font-weight:600; }
QLabel[role="infoValue"] { color:#F4F6FA; font-size:17px; font-weight:600; }
QLabel[role="infoValue"][tone="neutral"] { color:#C0CAD7; }
QLabel[role="infoValue"][tone="success"] { color:#7CE1B5; }
QLabel[role="infoValue"][tone="warning"] { color:#F7C66A; }
QLabel[role="infoValue"][tone="error"] { color:#FF929E; }
QLabel[role="badge"] { border-radius:13px; padding:6px 12px; font-size:17px; font-weight:600; }
QLabel[role="badge"][tone="neutral"] { background:#2A3042; color:#D0D7E1; border:1px solid #41485E; }
QLabel[role="badge"][tone="success"] { background:#183A32; color:#8FE0BC; border:1px solid #2B5B4E; }
QLabel[role="badge"][tone="warning"] { background:#44351E; color:#F5CB7A; border:1px solid #65502D; }
QLabel[role="badge"][tone="info"] { background:#203847; color:#9AD5E6; border:1px solid #365A6C; }
QLabel[role="emptyIcon"] { color:#7CC7F3; font-size:25px; font-weight:700; }
QLabel[role="emptyTitle"] { color:#F4F6FA; font-size:20px; font-weight:700; }
QLabel[role="emptyHint"] { color:#B8C2D0; font-size:17px; }
QLabel[role="sliderEdge"] { color:#B8C2D0; font-size:17px; }
QLabel[role="sliderValue"] { background:#24384A; color:#A8D8E7; border:1px solid #436278; border-radius:7px; padding:5px 8px; font-size:17px; font-weight:700; }
QWidget#deviceEmptyState { background:#15192A; border:1px solid #393D55; border-radius:8px; }
QStackedWidget#deviceContentStack { background:transparent; border:none; }
QLineEdit, QSpinBox, QPlainTextEdit, QTreeWidget {
    background:#111626; color:#F2F5F8; border:1px solid #414962; border-radius:8px;
    selection-background-color:#466087; selection-color:#FFFFFF;
}
QLineEdit, QSpinBox { min-height:32px; padding:7px 10px; }
QLineEdit:hover, QSpinBox:hover { border-color:#65718E; background:#171C2E; }
QLineEdit:focus, QSpinBox:focus, QPlainTextEdit:focus, QTreeWidget:focus { border:1px solid #64A7B2; }
QLineEdit:disabled, QSpinBox:disabled { background:#171B2A; color:#7D899B; }
QSpinBox::up-button, QSpinBox::down-button { width:24px; border:none; background:transparent; }
QCheckBox { color:#CDD5E1; spacing:7px; background:transparent; }
QCheckBox::indicator { width:20px; height:20px; border:1px solid #626D88; border-radius:4px; background:#111626; }
QCheckBox::indicator:hover { border-color:#79B8C2; }
QCheckBox::indicator:checked { background:#3D7B85; border-color:#63A5AF; image:none; }
QTreeWidget { padding:4px; outline:none; alternate-background-color:#171C2E; selection-background-color:#354564; selection-color:#FFFFFF; }
QTreeWidget::item { min-height:38px; border-radius:6px; padding:3px 7px; }
QTreeWidget::item:hover { background:#293149; color:#FFFFFF; }
QTreeWidget::item:selected { background:#354564; color:#FFFFFF; }
QHeaderView::section { background:#23243A; color:#CDD5E1; padding:10px 11px; border:none; border-bottom:1px solid #41435E; font-size:17px; font-weight:600; }
QPushButton { min-height:32px; background:#283149; color:#F1F4F8; border:1px solid #46516E; border-radius:8px; padding:8px 14px; font-weight:600; }
QPushButton:hover { background:#33405B; color:#FFFFFF; border-color:#66799E; }
QPushButton:pressed { background:#202B43; color:#FFFFFF; border-color:#6F9FA8; padding-top:10px; padding-bottom:6px; }
QPushButton:checked { background:#354E67; color:#FFFFFF; border-color:#72AAB3; }
QPushButton:disabled { background:#171B2A; color:#7D899B; border-color:#30364B; }
QPushButton[role="primary"] { background:#356D78; color:#FFFFFF; border-color:#4D8792; }
QPushButton[role="primary"]:hover { background:#3F7D88; color:#FFFFFF; border-color:#63A0AA; }
QPushButton[role="primary"]:pressed, QPushButton[role="primary"]:checked { background:#2E616C; color:#FFFFFF; border-color:#73AAB2; }
QPushButton[role="softPrimary"] { background:#24384A; color:#A8D8E7; border-color:#436278; }
QPushButton[role="softPrimary"]:hover { background:#2D465A; color:#D6F1F7; border-color:#5A7E91; }
QPushButton[role="danger"] { background:#3B242F; color:#FFB0BA; border-color:#69404D; }
QPushButton[role="danger"]:hover { background:#4A2C39; color:#FFD5DA; border-color:#915566; }
QPushButton[role="danger"]:pressed, QPushButton[role="danger"]:checked { background:#934B54; color:#FFFFFF; border-color:#B66A72; }
QPushButton[tone="recording"] { background:#B94955; color:#FFFFFF; border-color:#D66B76; }
QPushButton[tone="recording"]:hover { background:#D05A66; color:#FFFFFF; border-color:#EF8791; }
QPushButton[role="ptz"] { font-size:17px; padding:7px; }
QPushButton[role="ptz"][tone="stop"] { background:#3B242F; color:#FFB0BA; border-color:#69404D; }
QPushButton[role="ptz"][tone="stop"]:hover { background:#4A2C39; color:#FFD5DA; border-color:#915566; }
QPushButton[role="ptz"][tone="stop"]:pressed { background:#6E3940; color:#FFFFFF; border-color:#A95C65; }
QPushButton[role="layout"] { min-width:30px; min-height:30px; padding:6px 10px; font-size:17px; }
QPushButton#themeButton { background:#E9EDF4; color:#1A1B2E; border-color:#FFFFFF; }
QPushButton#themeButton:hover { background:#FFFFFF; color:#121321; }
QTabWidget::pane { background:#1B1F31; border:1px solid #393D55; border-radius:10px; top:-1px; }
QTabBar::tab { background:#15192A; color:#B8C2D0; border:1px solid #393D55; padding:7px 14px; margin-right:3px; border-top-left-radius:8px; border-top-right-radius:8px; }
QTabBar::tab:hover { background:#293149; color:#FFFFFF; }
QTabBar::tab:selected { background:#1B1F31; color:#D6F1F7; border-bottom-color:#1B1F31; font-weight:600; }
QFrame#videoFrame { background:#252B40; border:none; border-radius:9px; }
QStackedWidget#videoSurface { background:#070A0E; border-radius:7px; }
QLabel#liveView { background:#070A0E; border-radius:7px; color:#C3CDD7; }
QLabel#liveView[previewState="idle"] { background:#111827; color:#C3CDD7; font-size:17px; }
QLabel#liveView[previewState="active"] { background:#05080B; color:#C3CDD7; }
QWidget#videoGrid { background:#070A0E; }
QLabel#videoGridEmptyState { background:#070A0E; color:#C3CDD7; font-size:17px; }
QFrame#videoTile { background:#111626; border:2px solid #3B435B; border-radius:8px; }
QFrame#videoTile[active="true"] { border:2px solid #72B7C2; }
QFrame#videoTileHeader, QFrame#videoTileFooter { background:#1A2032; border:none; }
QLabel#videoTileSurface { background:#030609; color:#C3CDD7; border:none; }
QLabel[role="videoTitle"] { color:#F3F6FA; font-weight:700; }
QLabel[role="videoStatus"][tone="neutral"] { color:#AAB5C4; }
QLabel[role="videoStatus"][tone="info"] { color:#91D5E0; }
QLabel[role="videoStatus"][tone="success"] { color:#8FE0BC; }
QLabel[role="videoStatus"][tone="warning"] { color:#F5CB7A; }
QLabel[role="videoHint"] { color:#8B96A8; font-size:15px; }
QSlider::groove:horizontal { height:6px; background:#414960; border-radius:3px; }
QSlider::sub-page:horizontal { background:#4C8A94; border-radius:3px; }
QSlider::add-page:horizontal { background:#414960; border-radius:3px; }
QSlider::handle:horizontal { width:18px; height:18px; margin:-6px 0; background:#F0F3F8; border:2px solid #4C8A94; border-radius:9px; }
QSlider::handle:horizontal:hover { background:#FFFFFF; border-color:#72B0BA; }
QPlainTextEdit#logView { font-family:"Consolas", "Microsoft YaHei UI"; font-size:17px; padding:10px; }
QSplitter::handle { background:transparent; width:8px; height:8px; }
QSplitter::handle:hover { background:#414960; border-radius:3px; }
QScrollBar:vertical { background:transparent; width:10px; margin:2px; }
QScrollBar:horizontal { background:transparent; height:10px; margin:2px; }
QScrollBar::handle { background:#4B536B; border-radius:4px; min-height:28px; min-width:28px; }
QScrollBar::handle:hover { background:#65718E; }
QScrollBar::add-line, QScrollBar::sub-line { width:0; height:0; }
QToolTip { background:#F1F4F8; color:#181A2A; border:none; padding:6px; }
)QSS");
}

}  // namespace client_theme

#endif
