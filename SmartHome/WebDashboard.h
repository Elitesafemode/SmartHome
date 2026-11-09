#pragma once

#include <Arduino.h>
#include <pgmspace.h>

const char DASHBOARD_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="fa" dir="rtl">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, viewport-fit=cover">
<meta name="theme-color" content="#1d4a5a">
<meta name="color-scheme" content="light dark">

<title>خانه هوشمند</title>

<style>
:root {
    --bg: #edf0f3;
    --surface: #ffffff;
    --sunken: #e4e9ee;
    --ink: #16222f;
    --muted: #5d6c7b;
    --line: #d8dfe6;
    --brand: #1d4a5a;
    --brand-soft: #dcebf0;
    --ok: #1b8a57;
    --ok-soft: #dff3e8;
    --warn: #b86e00;
    --warn-soft: #fbefd6;
    --bad: #c4352b;
    --bad-soft: #fbe3e0;
    --shadow: 0 1px 2px rgba(22, 34, 47, .06), 0 6px 18px rgba(22, 34, 47, .05);
    --c-temp: #d9582b;
    --c-hum: #2c78d6;
    --c-rain: #14958a;
    --radius: 16px;
}

@media (prefers-color-scheme: dark) {
    :root:not([data-theme="light"]) {
        --bg: #0e151c;
        --surface: #16202a;
        --sunken: #0e151c;
        --ink: #e7edf2;
        --muted: #8b9aa9;
        --line: #26333f;
        --brand: #6bbcd1;
        --brand-soft: #1b3440;
        --ok: #3fbf84;
        --ok-soft: #14301f;
        --warn: #e6a53c;
        --warn-soft: #382a10;
        --bad: #f2675d;
        --bad-soft: #3b1a18;
        --shadow: 0 1px 2px rgba(0, 0, 0, .4);
        --c-temp: #ff8c61;
        --c-hum: #6fb2ff;
        --c-rain: #4fd3c2;
    }
}

:root[data-theme="dark"] {
    --bg: #0e151c;
    --surface: #16202a;
    --sunken: #0e151c;
    --ink: #e7edf2;
    --muted: #8b9aa9;
    --line: #26333f;
    --brand: #6bbcd1;
    --brand-soft: #1b3440;
    --ok: #3fbf84;
    --ok-soft: #14301f;
    --warn: #e6a53c;
    --warn-soft: #382a10;
    --bad: #f2675d;
    --bad-soft: #3b1a18;
    --shadow: 0 1px 2px rgba(0, 0, 0, .4);
    --c-temp: #ff8c61;
    --c-hum: #6fb2ff;
    --c-rain: #4fd3c2;
}

* { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }

html { color-scheme: light dark; }

body {
    margin: 0;
    background: var(--bg);
    color: var(--ink);
    font-family: Vazirmatn, "Segoe UI", system-ui, -apple-system, Tahoma, Arial, sans-serif;
    font-size: 15px;
    line-height: 1.7;
    direction: rtl;
    font-variant-numeric: tabular-nums;
}

.hidden { display: none !important; }

svg.i {
    width: 22px;
    height: 22px;
    fill: none;
    stroke: currentColor;
    stroke-width: 1.7;
    stroke-linecap: round;
    stroke-linejoin: round;
    flex: none;
}

:focus-visible {
    outline: 2px solid var(--brand);
    outline-offset: 2px;
}

/* ---------- Header ---------- */
header {
    position: sticky;
    top: 0;
    z-index: 20;
    background: var(--surface);
    border-bottom: 1px solid var(--line);
    padding: 12px 20px;
    padding-top: calc(12px + env(safe-area-inset-top, 0px));
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: 12px;
}

.brand { display: flex; align-items: center; gap: 12px; min-width: 0; }

.brand .mark {
    width: 42px;
    height: 42px;
    border-radius: 12px;
    background: var(--brand);
    color: var(--surface);
    display: grid;
    place-items: center;
}

.brand h1 { font-size: 17px; margin: 0; line-height: 1.3; }
.brand small { color: var(--muted); font-size: 12px; display: block; }

.tools { display: flex; align-items: center; gap: 8px; }

.status {
    display: inline-flex;
    align-items: center;
    gap: 7px;
    border-radius: 99px;
    padding: 3px 11px;
    font-size: 12.5px;
    font-weight: 600;
    background: var(--sunken);
    color: var(--muted);
    white-space: nowrap;
}

.status::before {
    content: "";
    width: 7px;
    height: 7px;
    border-radius: 50%;
    background: currentColor;
}

.status.good { background: var(--ok-soft); color: var(--ok); }
.status.bad { background: var(--bad-soft); color: var(--bad); }
.status.warning { background: var(--warn-soft); color: var(--warn); }

/* ---------- Buttons ---------- */
button { font-family: inherit; }

.btn {
    border: 1px solid transparent;
    border-radius: 11px;
    padding: 9px 16px;
    font-size: 14px;
    font-weight: 700;
    cursor: pointer;
    color: var(--ink);
    background: var(--sunken);
    transition: filter .15s, background .15s;
}

.btn:hover:not(:disabled) { filter: brightness(.96); }
.btn.primary { background: var(--brand); color: var(--surface); }
.btn.danger { background: var(--bad-soft); color: var(--bad); }
.btn.block { width: 100%; padding: 12px; }
.btn:disabled { opacity: .45; cursor: not-allowed; }

.icon-btn {
    width: 40px;
    height: 40px;
    padding: 0;
    display: grid;
    place-items: center;
    border-radius: 11px;
    border: 1px solid var(--line);
    background: transparent;
    color: var(--ink);
    cursor: pointer;
}

.actions { display: flex; flex-wrap: wrap; gap: 8px; margin-top: 14px; }

/* ---------- Layout ---------- */
.container { max-width: 1160px; margin: 0 auto; padding: 22px 20px 40px; }

.shell {
    display: grid;
    grid-template-columns: 210px minmax(0, 1fr);
    gap: 22px;
    align-items: start;
}

.nav {
    position: sticky;
    top: 86px;
    display: flex;
    flex-direction: column;
    gap: 4px;
}

.nav button {
    display: flex;
    align-items: center;
    gap: 11px;
    border: 0;
    background: transparent;
    color: var(--muted);
    padding: 11px 14px;
    border-radius: 12px;
    font-size: 14.5px;
    font-weight: 600;
    cursor: pointer;
    text-align: start;
}

.nav button:hover { background: var(--surface); color: var(--ink); }
.nav button.active { background: var(--brand-soft); color: var(--brand); }

.section { display: none; }
.section.active { display: block; }

/* ---------- Panels ---------- */
.panel {
    background: var(--surface);
    border: 1px solid var(--line);
    border-radius: var(--radius);
    box-shadow: var(--shadow);
    padding: 20px;
    margin-bottom: 18px;
}

.panel h2 { font-size: 16px; margin: 0 0 4px; }
.panel .lead { color: var(--muted); font-size: 13.5px; margin: 0 0 16px; }
.panel p { margin: 8px 0; }

.group-title {
    font-size: 13.5px;
    color: var(--muted);
    font-weight: 700;
    margin: 22px 2px 10px;
}

.group-title:first-child { margin-top: 0; }

/* ---------- Hero ---------- */
.hero {
    border-radius: var(--radius);
    padding: 22px;
    margin-bottom: 8px;
    background: var(--ok-soft);
    color: var(--ok);
    border: 1px solid transparent;
    display: flex;
    flex-wrap: wrap;
    align-items: center;
    justify-content: space-between;
    gap: 16px;
}

.hero[data-tone="warn"] { background: var(--warn-soft); color: var(--warn); }
.hero[data-tone="bad"] { background: var(--bad-soft); color: var(--bad); }
.hero[data-tone="idle"] { background: var(--surface); color: var(--muted); border-color: var(--line); }

.hero-main { display: flex; align-items: center; gap: 16px; min-width: 0; }

.hero-main .ring {
    width: 54px;
    height: 54px;
    border-radius: 50%;
    display: grid;
    place-items: center;
    background: var(--surface);
    flex: none;
}

.hero-main .ring svg { width: 28px; height: 28px; }

.hero h2 { margin: 0; font-size: 22px; line-height: 1.4; color: var(--ink); }
.hero p { margin: 2px 0 0; color: var(--ink); opacity: .75; font-size: 14px; }

.hero-chips { display: flex; flex-wrap: wrap; gap: 8px; }

.chip {
    background: var(--surface);
    color: var(--ink);
    border-radius: 99px;
    padding: 5px 13px;
    font-size: 13px;
}

.chip b { font-weight: 700; }

/* ---------- Sensor cards ---------- */
.grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(200px, 1fr)); gap: 12px; }

.card {
    background: var(--surface);
    border: 1px solid var(--line);
    border-radius: 14px;
    padding: 15px 16px;
    min-width: 0;
    position: relative;
    border-inline-start: 4px solid var(--line);
}

.card[data-tone="warn"] { border-inline-start-color: var(--warn); }
.card[data-tone="bad"] { border-inline-start-color: var(--bad); background: var(--bad-soft); }
.card[data-tone="good"] { border-inline-start-color: var(--ok); }

.card .name {
    display: flex;
    align-items: center;
    gap: 8px;
    color: var(--muted);
    font-size: 13px;
    font-weight: 600;
}

.card .name svg { width: 18px; height: 18px; }

.card .value {
    font-size: 21px;
    font-weight: 800;
    margin-top: 10px;
    overflow-wrap: anywhere;
    line-height: 1.4;
}

.card .value.long { font-size: 16px; }
.card .status { margin-top: 10px; }

/* ---------- Rows ---------- */
.row {
    display: flex;
    align-items: center;
    justify-content: space-between;
    flex-wrap: wrap;
    gap: 10px;
    padding: 13px 0;
    border-bottom: 1px solid var(--line);
}

.row:last-child { border-bottom: 0; padding-bottom: 0; }
.row:first-of-type { padding-top: 0; }
.row > span { color: var(--muted); font-size: 14px; }
.row > strong { font-size: 14px; }

.dev { display: flex; align-items: center; gap: 13px; }

.dev .ico {
    width: 42px;
    height: 42px;
    border-radius: 12px;
    background: var(--sunken);
    display: grid;
    place-items: center;
    color: var(--muted);
    transition: background .2s, color .2s;
}

.dev.on .ico { background: var(--brand-soft); color: var(--brand); }
.dev b { display: block; font-size: 15px; }
.dev small { color: var(--muted); font-size: 13px; }

/* ---------- Switch & segmented ---------- */
.switch {
    width: 52px;
    height: 30px;
    border: 0;
    padding: 0;
    border-radius: 99px;
    background: var(--line);
    position: relative;
    cursor: pointer;
    transition: background .2s;
    flex: none;
}

.switch::after {
    content: "";
    position: absolute;
    top: 3px;
    inset-inline-start: 3px;
    width: 24px;
    height: 24px;
    border-radius: 50%;
    background: #fff;
    box-shadow: 0 1px 3px rgba(0, 0, 0, .3);
    transition: inset-inline-start .2s;
}

.switch[aria-checked="true"] { background: var(--ok); }
.switch[aria-checked="true"]::after { inset-inline-start: 25px; }
.switch:disabled { opacity: .4; cursor: not-allowed; }

.seg {
    display: flex;
    gap: 4px;
    padding: 4px;
    border-radius: 14px;
    background: var(--sunken);
}

.seg button {
    flex: 1;
    border: 0;
    background: transparent;
    color: var(--muted);
    padding: 11px 8px;
    border-radius: 10px;
    font-size: 14px;
    font-weight: 700;
    cursor: pointer;
    transition: background .15s, color .15s;
}

.seg button.on {
    background: var(--surface);
    color: var(--brand);
    box-shadow: 0 1px 3px rgba(0, 0, 0, .15);
}

/* ---------- Notices & messages ---------- */
.notice {
    display: flex;
    gap: 10px;
    align-items: flex-start;
    background: var(--sunken);
    color: var(--muted);
    border-radius: 12px;
    padding: 11px 14px;
    font-size: 13.5px;
    margin: 0 0 14px;
}

.notice.active { background: var(--warn-soft); color: var(--warn); }
.notice svg { width: 18px; height: 18px; margin-top: 3px; }

.message { color: var(--warn); font-size: 13.5px; margin-top: 12px; min-height: 1px; }

/* ---------- Security mode descriptions ---------- */
.security-mode-info {
    display: grid;
    grid-template-columns: repeat(3, minmax(0, 1fr));
    gap: 10px;
    margin-top: 14px;
}

.mode-card {
    background: var(--sunken);
    border: 1px solid var(--line);
    border-radius: 12px;
    padding: 13px;
    min-width: 0;
}

.mode-card h3 {
    font-size: 14px;
    margin: 0 0 6px;
    color: var(--ink);
}

.mode-card p {
    font-size: 12.5px;
    color: var(--muted);
    margin: 0;
    line-height: 1.8;
}

.mode-card .mode-label {
    display: inline-block;
    font-size: 11px;
    font-weight: 700;
    border-radius: 20px;
    padding: 2px 8px;
    margin-bottom: 7px;
    background: var(--brand-soft);
    color: var(--brand);
}

/* ---------- About project ---------- */
.about-intro {
    background: var(--brand-soft);
    border-radius: 14px;
    padding: 18px;
    margin-bottom: 18px;
}

.about-intro h2 {
    margin: 0 0 8px;
    color: var(--brand);
    font-size: 19px;
}

.about-intro p {
    margin: 0;
    color: var(--ink);
    font-size: 14px;
}

.about-grid {
    display: grid;
    grid-template-columns: repeat(2, minmax(0, 1fr));
    gap: 12px;
}

.about-card {
    background: var(--surface);
    border: 1px solid var(--line);
    border-radius: 14px;
    padding: 16px;
    min-width: 0;
}

.about-card h3 {
    display: flex;
    align-items: center;
    gap: 9px;
    font-size: 14.5px;
    margin: 0 0 10px;
}

.about-card h3 svg {
    color: var(--brand);
    width: 20px;
    height: 20px;
}

.about-card ul {
    margin: 0;
    padding-right: 19px;
    color: var(--muted);
    font-size: 13px;
}

.about-card li {
    padding: 3px 0;
}

.about-note {
    margin-top: 14px;
    padding: 12px 14px;
    background: var(--warn-soft);
    color: var(--warn);
    border-radius: 12px;
    font-size: 12.5px;
}

/* ---------- Forms ---------- */
label { display: block; font-size: 13.5px; font-weight: 600; margin: 14px 0 6px; }

input {
    width: 100%;
    padding: 12px 14px;
    border-radius: 11px;
    border: 1px solid var(--line);
    background: var(--sunken);
    color: var(--ink);
    font-family: inherit;
    font-size: 15px;
}

input:focus { outline: 2px solid var(--brand); outline-offset: 0; background: var(--surface); }

.login-box { max-width: 420px; margin: 40px auto; padding: 28px; }
.login-box .mark {
    width: 52px;
    height: 52px;
    border-radius: 15px;
    background: var(--brand);
    color: var(--surface);
    display: grid;
    place-items: center;
    margin-bottom: 18px;
}

.login-box .btn { margin-top: 20px; }

/* ---------- Lists ---------- */
#historyList, #sessionsList { margin-top: 16px; }

.event {
    position: relative;
    padding: 12px 22px 12px 0;
    border-bottom: 1px solid var(--line);
    font-size: 14px;
}

.event::before {
    content: "";
    position: absolute;
    inset-inline-start: 0;
    top: 21px;
    width: 8px;
    height: 8px;
    border-radius: 50%;
    background: var(--brand);
}

.event { padding-inline-start: 22px; padding-inline-end: 0; }
.event:last-child { border-bottom: 0; }
.event small { display: block; color: var(--muted); margin-top: 3px; font-size: 12.5px; }

footer { text-align: center; color: var(--muted); font-size: 12px; padding: 4px 20px 24px; }

/* ---------- Trend charts ---------- */
.chart-bar {
    display: flex;
    flex-wrap: wrap;
    align-items: center;
    gap: 12px 18px;
    margin-bottom: 12px;
}

.chart-bar .seg { min-width: 270px; }
.chart-note { color: var(--muted); font-size: 12.5px; flex: 1 1 220px; }

.charts {
    display: grid;
    grid-template-columns: repeat(2, minmax(0, 1fr));
    gap: 12px;
}

.chart-card {
    background: var(--surface);
    border: 1px solid var(--line);
    border-radius: 14px;
    padding: 16px 16px 14px;
    min-width: 0;
}

.chart-card.wide { grid-column: 1 / -1; }

.chart-head {
    display: flex;
    align-items: flex-start;
    justify-content: space-between;
    gap: 10px;
    margin-bottom: 6px;
}

.chart-head .name {
    display: flex;
    align-items: center;
    gap: 8px;
    color: var(--muted);
    font-size: 13px;
    font-weight: 600;
}

.chart-head .name svg { width: 18px; height: 18px; color: var(--c); }

.chart-head .now { text-align: end; line-height: 1.3; }
.chart-head .now b { font-size: 24px; font-weight: 800; color: var(--ink); }
.chart-head .now small { display: block; color: var(--muted); font-size: 12px; }

.chart-wrap { position: relative; direction: ltr; }

.ch-svg {
    display: block;
    width: 100%;
    height: 176px;
    direction: ltr;
    touch-action: pan-y;
    cursor: crosshair;
    user-select: none;
    -webkit-user-select: none;
}

.ch-svg .ax { fill: var(--muted); font-size: 11px; font-family: inherit; }
.ch-svg .gl { stroke: var(--line); stroke-width: 1; }
.ch-svg .gl.base { stroke: var(--muted); opacity: .35; }
.ch-svg .ln { fill: none; stroke: var(--c); stroke-width: 2.2; stroke-linecap: round; stroke-linejoin: round; }
.ch-svg .dot { fill: var(--c); stroke: var(--surface); stroke-width: 2; }
.ch-svg .band { fill: var(--warn); opacity: .16; }
.ch-svg .hv line { stroke: var(--muted); stroke-width: 1; stroke-dasharray: 3 3; }

.ch-svg .ring {
    fill: var(--c);
    transform-box: fill-box;
    transform-origin: center;
    animation: ping 1.9s ease-out infinite;
}

@keyframes ping {
    from { transform: scale(1); opacity: .45; }
    to { transform: scale(3.2); opacity: 0; }
}

.chart-tip {
    position: absolute;
    top: -4px;
    z-index: 2;
    display: flex;
    gap: 10px;
    align-items: center;
    direction: rtl;
    background: var(--ink);
    color: var(--surface);
    border-radius: 9px;
    padding: 4px 10px;
    font-size: 12.5px;
    white-space: nowrap;
    pointer-events: none;
}

.chart-tip[hidden] { display: none; }
.chart-tip b { font-weight: 800; }

.chart-stats {
    display: flex;
    flex-wrap: wrap;
    gap: 6px 22px;
    margin-top: 10px;
    padding-top: 10px;
    border-top: 1px solid var(--line);
}

.chart-stats div { min-width: 0; }
.chart-stats small { display: block; color: var(--muted); font-size: 12px; }
.chart-stats b { font-size: 14.5px; }

.chart-legend {
    display: flex;
    align-items: center;
    gap: 7px;
    margin-top: 8px;
    color: var(--muted);
    font-size: 12px;
}

.chart-legend i {
    width: 14px;
    height: 10px;
    border-radius: 3px;
    background: var(--warn);
    opacity: .35;
}

@media (max-width: 800px) {
    .charts { grid-template-columns: 1fr; }
    .chart-bar .seg { min-width: 0; width: 100%; }
}

/* ---------- Mobile ---------- */
@media (max-width: 800px) {
    .shell { display: block; }

    .container { padding: 14px 14px 100px; }

    .nav {
        position: fixed;
        inset-inline: 0;
        bottom: 0;
        top: auto;
        z-index: 30;
        flex-direction: row;
        gap: 0;
        background: var(--surface);
        border-top: 1px solid var(--line);
        padding: 6px 4px calc(6px + env(safe-area-inset-bottom, 0px));
        overflow-x: auto;
    }

    .nav button {
        flex: 1 0 0;
        min-width: 0;
        flex-direction: column;
        gap: 3px;
        padding: 7px 1px;
        font-size: 10px;
        text-align: center;
        justify-content: center;
        white-space: nowrap;
    }

    .nav button svg { width: 20px; height: 20px; }

    header { padding-inline: 14px; }
    .brand small { display: none; }
    .hero h2 { font-size: 19px; }
    .grid { grid-template-columns: repeat(2, minmax(0, 1fr)); gap: 10px; }
    .card { padding: 13px; }
    .card .value { font-size: 18px; }
    .login-box { margin: 16px auto; }
    footer { padding-bottom: 90px; }

    .security-mode-info { grid-template-columns: 1fr; }
    .about-grid { grid-template-columns: 1fr; }
}

@media (prefers-reduced-motion: reduce) {
    * { transition: none !important; }
    .ch-svg .ring { animation: none; opacity: 0; }
}
</style>
</head>

<body>

<!-- Icon set -->
<svg width="0" height="0" style="position:absolute" aria-hidden="true">
    <symbol id="i-home" viewBox="0 0 24 24"><path d="M3 11l9-8 9 8M5 9.5V20h14V9.5M10 20v-5h4v5"/></symbol>
    <symbol id="i-temp" viewBox="0 0 24 24"><path d="M14 14.8V5a2 2 0 1 0-4 0v9.8a4 4 0 1 0 4 0z"/></symbol>
    <symbol id="i-drop" viewBox="0 0 24 24"><path d="M12 3s6 6.5 6 11a6 6 0 0 1-12 0c0-4.5 6-11 6-11z"/></symbol>
    <symbol id="i-gas" viewBox="0 0 24 24"><path d="M12 3c1 3.5 5 5.5 5 10a5 5 0 0 1-10 0c0-2 1-3.2 2-4.2.3 1.4 1 2 1.8 2.2C11 8.5 11 5.5 12 3z"/></symbol>
    <symbol id="i-motion" viewBox="0 0 24 24"><circle cx="12" cy="12" r="2"/><path d="M7.8 7.8a6 6 0 0 0 0 8.4M16.2 7.8a6 6 0 0 1 0 8.4M4.9 4.9a10 10 0 0 0 0 14.2M19.1 4.9a10 10 0 0 1 0 14.2"/></symbol>
    <symbol id="i-door" viewBox="0 0 24 24"><path d="M6 21V4a1 1 0 0 1 1-1h10a1 1 0 0 1 1 1v17M4 21h16M14.5 12.5v.01"/></symbol>
    <symbol id="i-garage" viewBox="0 0 24 24"><path d="M3 21V9l9-6 9 6v12M7 21v-8h10v8M7 17h10"/></symbol>
    <symbol id="i-rain" viewBox="0 0 24 24"><path d="M7 15a4.5 4.5 0 0 1-.5-9A6 6 0 0 1 18 7.5 3.8 3.8 0 0 1 17 15M9 18l-1 3M13 18l-1 3M17 18l-1 3"/></symbol>
    <symbol id="i-sun" viewBox="0 0 24 24"><circle cx="12" cy="12" r="4"/><path d="M12 2v2M12 20v2M2 12h2M20 12h2M4.9 4.9l1.4 1.4M17.7 17.7l1.4 1.4M4.9 19.1l1.4-1.4M17.7 6.3l1.4-1.4"/></symbol>
    <symbol id="i-moon" viewBox="0 0 24 24"><path d="M20 14.5A8 8 0 0 1 9.5 4a8 8 0 1 0 10.5 10.5z"/></symbol>
    <symbol id="i-fan" viewBox="0 0 24 24"><circle cx="12" cy="12" r="1.5"/><path d="M12 10.5C12 6 10 3 7.5 4.5S8 9.5 10.8 11M13.5 12c4.5 0 7.5-2 6-4.5S14.5 8 13 10.8M12 13.5c0 4.5 2 7.5 4.5 6S16 14.5 13.2 13M10.5 12C6 12 3 14 4.5 16.5S9.5 16 11 13.2"/></symbol>
    <symbol id="i-bulb" viewBox="0 0 24 24"><path d="M9 18h6M10 21h4M12 3a6 6 0 0 0-3.5 10.8c.6.5 1 1.2 1 2V16h5v-.2c0-.8.4-1.5 1-2A6 6 0 0 0 12 3z"/></symbol>
    <symbol id="i-heat" viewBox="0 0 24 24"><path d="M8 20c-2-2-2-4 0-6s2-4 0-6M13 20c-2-2-2-4 0-6s2-4 0-6M18 20c-2-2-2-4 0-6s2-4 0-6"/></symbol>
    <symbol id="i-lock" viewBox="0 0 24 24"><rect x="5" y="11" width="14" height="10" rx="2"/><path d="M8 11V8a4 4 0 0 1 8 0v3"/></symbol>
    <symbol id="i-shield" viewBox="0 0 24 24"><path d="M12 3l8 3v6c0 5-3.500 8-8 9-4.500-1-8-4-8-9V6z"/></symbol>
    <symbol id="i-auto" viewBox="0 0 24 24"><path d="M13 2L4 14h7l-1 8 9-12h-7z"/></symbol>
    <symbol id="i-device" viewBox="0 0 24 24"><rect x="7" y="2" width="10" height="20" rx="2"/><path d="M11 18h2"/></symbol>
    <symbol id="i-history" viewBox="0 0 24 24"><path d="M3 12a9 9 0 1 0 3-6.700M3 4v5h5M12 7v5l3 2"/></symbol>
    <symbol id="i-check" viewBox="0 0 24 24"><path d="M5 12.500l4.500 4.500L19 7.500"/></symbol>
    <symbol id="i-alert" viewBox="0 0 24 24"><path d="M12 3l10 18H2zM12 10v5M12 18v.01"/></symbol>
    <symbol id="i-info" viewBox="0 0 24 24"><circle cx="12" cy="12" r="9"/><path d="M12 11v6M12 7.500v.01"/></symbol>
</svg>

<header>
    <div class="brand">
        <div class="mark"><svg class="i"><use href="#i-home"/></svg></div>
        <div>
            <h1>خانه هوشمند</h1>
            <small>مدیریت و پایش محلی ساختمان</small>
        </div>
    </div>

    <div class="tools">
        <span id="connectionStatus" class="status warning">در انتظار اتصال</span>

        <button id="themeButton" class="icon-btn" onclick="toggleTheme()" aria-label="تغییر تم">
            <svg class="i"><use id="themeIcon" href="#i-moon"/></svg>
        </button>

        <button id="logoutButton" class="btn hidden" onclick="logout()">خروج</button>
    </div>
</header>

<div class="container">

    <!-- Initial administrator setup -->
    <section id="setupScreen" class="panel login-box hidden">
        <div class="mark"><svg class="i"><use href="#i-lock"/></svg></div>
        <h2>راه‌اندازی اولیه</h2>
        <p class="lead">برای استفاده از خانه هوشمند، رمز مدیر را تعریف کن. این مرحله فقط یک‌بار انجام می‌شود.</p>
        <label for="setupPassword">رمز عبور جدید (حداقل ۱۰ کاراکتر)</label>
        <input type="password" id="setupPassword" autocomplete="new-password" placeholder="رمز عبور مدیر">
        <label for="setupConfirmation">تکرار رمز عبور</label>
        <input type="password" id="setupConfirmation" autocomplete="new-password" placeholder="تکرار رمز عبور">
        <button class="btn primary block" onclick="setupAccount()">ثبت رمز و ادامه</button>
        <div id="setupMessage" class="message"></div>
    </section>

    <!-- Login -->
    <section id="loginScreen" class="panel login-box hidden">
        <div class="mark"><svg class="i"><use href="#i-home"/></svg></div>
        <h2>ورود مدیر سیستم</h2>
        <p class="lead">برای مشاهده و کنترل خانه، رمز عبور را وارد کن.</p>
        <label for="password">رمز عبور</label>
        <input type="password" id="password" autocomplete="current-password" placeholder="رمز عبور مدیر">
        <button class="btn primary block" onclick="login()">ورود به داشبورد</button>
        <div id="loginMessage" class="message"></div>
    </section>

    <!-- Dashboard -->
    <main id="dashboard" class="shell hidden">

        <nav class="nav">
            <button class="active" onclick="showSection('home',this)">
                <svg class="i"><use href="#i-home"/></svg>نمای کلی
            </button>
            <button onclick="showSection('controls',this)">
                <svg class="i"><use href="#i-bulb"/></svg>تجهیزات
            </button>
            <button onclick="showSection('security',this)">
                <svg class="i"><use href="#i-shield"/></svg>امنیت
            </button>
            <button onclick="showSection('automation',this)">
                <svg class="i"><use href="#i-auto"/></svg>اتوماسیون
            </button>
            <button onclick="showSection('access',this)">
                <svg class="i"><use href="#i-device"/></svg>دسترسی
            </button>
            <button onclick="showSection('history',this)">
                <svg class="i"><use href="#i-history"/></svg>تاریخچه
            </button>
            <button onclick="showSection('about',this)">
                <svg class="i"><use href="#i-info"/></svg>درباره پروژه
            </button>
        </nav>

        <div>

        <!-- Home -->
        <section id="home" class="section active">

            <div id="hero" class="hero" data-tone="idle">
                <div class="hero-main">
                    <div class="ring"><svg class="i"><use id="heroIcon" href="#i-info"/></svg></div>
                    <div>
                        <h2 id="heroTitle">در حال دریافت وضعیت</h2>
                        <p id="heroSub">چند لحظه صبر کن</p>
                    </div>
                </div>
                <div class="hero-chips">
                    <span class="chip">حالت امنیتی: <b id="heroSec">--</b></span>
                    <span class="chip">حالت هوشمند: <b id="heroSmart">--</b></span>
                </div>
            </div>

            <div class="group-title">هوا و محیط</div>
            <div class="grid">

                <div class="card">
                    <div class="name"><svg class="i"><use href="#i-temp"/></svg>دمای محیط</div>
                    <div class="value" id="temperature">--</div>
                    <span class="status" id="dhtStatus">نامشخص</span>
                </div>

                <div class="card">
                    <div class="name"><svg class="i"><use href="#i-drop"/></svg>رطوبت محیط</div>
                    <div class="value" id="humidity">--</div>
                </div>

                <div class="card">
                    <div class="name"><svg class="i"><use href="#i-gas"/></svg>سنسور گاز</div>
                    <div class="value long" id="gas">--</div>
                    <span class="status" id="mq2Status">نامشخص</span>
                </div>

                <div class="card">
                    <div class="name"><svg class="i"><use href="#i-rain"/></svg>وضعیت باران</div>
                    <div class="value long" id="rain">--</div>
                    <span class="status" id="rainStatus">نامشخص</span>
                </div>

                <div class="card">
                    <div class="name"><svg class="i"><use href="#i-sun"/></svg>وضعیت روشنایی</div>
                    <div class="value long" id="ldr">--</div>
                    <span class="status" id="ldrStatus">نامشخص</span>
                </div>

            </div>

            <div class="group-title">درب‌ها و حرکت</div>
            <div class="grid">

                <div class="card">
                    <div class="name"><svg class="i"><use href="#i-door"/></svg>درب اصلی</div>
                    <div class="value" id="mainDoor">--</div>
                </div>

                <div class="card">
                    <div class="name"><svg class="i"><use href="#i-garage"/></svg>درب پارکینگ</div>
                    <div class="value" id="garageDoor">--</div>
                </div>

                <div class="card">
                    <div class="name"><svg class="i"><use href="#i-motion"/></svg>سنسور حرکت</div>
                    <div class="value long" id="pir">--</div>
                    <span class="status" id="pirStatus">نامشخص</span>
                </div>

            </div>

            <div class="group-title">روند تغییرات</div>
            <div class="chart-bar">
                <div class="seg">
                    <button class="on" onclick="setChartRange(300,this)">۵ دقیقه</button>
                    <button onclick="setChartRange(900,this)">۱۵ دقیقه</button>
                    <button onclick="setChartRange(1800,this)">۳۰ دقیقه</button>
                </div>
                <span class="chart-note">نمودارها از لحظه‌ی باز شدن داشبورد ساخته می‌شوند. برای دیدن مقدار دقیق، روی نمودار انگشت یا نشانگر ماوس را حرکت بده.</span>
            </div>

            <div class="charts">

                <div class="chart-card" style="--c:var(--c-temp)">
                    <div class="chart-head">
                        <div class="name"><svg class="i"><use href="#i-temp"/></svg>دمای محیط</div>
                        <div class="now"><b id="chNowTemp">--</b></div>
                    </div>
                    <div class="chart-wrap" id="chartTemp"><svg class="ch-svg" role="img" aria-label="نمودار دما"></svg><div class="chart-tip" hidden></div></div>
                    <div class="chart-stats" id="statsTemp"></div>
                </div>

                <div class="chart-card" style="--c:var(--c-hum)">
                    <div class="chart-head">
                        <div class="name"><svg class="i"><use href="#i-drop"/></svg>رطوبت محیط</div>
                        <div class="now"><b id="chNowHum">--</b></div>
                    </div>
                    <div class="chart-wrap" id="chartHum"><svg class="ch-svg" role="img" aria-label="نمودار رطوبت"></svg><div class="chart-tip" hidden></div></div>
                    <div class="chart-stats" id="statsHum"></div>
                </div>

                <div class="chart-card wide" style="--c:var(--c-rain)">
                    <div class="chart-head">
                        <div class="name"><svg class="i"><use href="#i-rain"/></svg>باران (درصد رطوبت سنسور)</div>
                        <div class="now"><b id="chNowRain">--</b><small id="chStateRain"></small></div>
                    </div>
                    <div class="chart-wrap" id="chartRain"><svg class="ch-svg" role="img" aria-label="نمودار باران"></svg><div class="chart-tip" hidden></div></div>
                    <div class="chart-stats" id="statsRain"></div>
                    <div class="chart-legend"><i></i>بازه‌هایی که بارندگی تشخیص داده شده</div>
                </div>

            </div>

            <div class="panel" style="margin-top:22px">
                <h2>وضعیت سیستم</h2>
                <p class="lead">خلاصه حالت‌های امنیتی، گاز و زمان کارکرد دستگاه</p>

                <div class="row">
                    <span>حالت هوشمند</span>
                    <strong id="smartStatus">--</strong>
                </div>

                <div class="row">
                    <span>حالت امنیتی</span>
                    <strong id="securityStatus">--</strong>
                </div>

                <div class="row">
                    <span>هشدار امنیتی</span>
                    <strong id="securityAlert">--</strong>
                </div>

                <div class="row">
                    <span>هشدار گاز</span>
                    <strong id="gasAlertStatus">--</strong>
                </div>

                <div class="row">
                    <span>خروجی شبیه‌ساز شیر گاز</span>
                    <strong id="gasValveOutput">--</strong>
                </div>

                <div class="row">
                    <span>قفل بازگشایی خروجی گاز</span>
                    <strong id="gasLock">--</strong>
                </div>

                <div class="row">
                    <span>زمان روشن بودن سیستم</span>
                    <strong id="uptime">--</strong>
                </div>
            </div>

        </section>

        <!-- Controls -->
        <section id="controls" class="section">

            <div class="panel">
                <h2>کنترل دستی تجهیزات</h2>
                <p class="lead">هر کلید فرمان را بلافاصله به دستگاه می‌فرستد.</p>

                <div id="smartNotice" class="notice">
                    <svg class="i"><use href="#i-info"/></svg>
                    <span>در حالت هوشمند، کنترل دستی تجهیزات غیرفعال است.</span>
                </div>

                <div class="row">
                    <div class="dev" id="devFan">
                        <div class="ico"><svg class="i"><use href="#i-fan"/></svg></div>
                        <div><b>فن</b><small id="fanState">نامشخص</small></div>
                    </div>
                    <button id="swFan" class="switch" role="switch" aria-checked="false" aria-label="فن" onclick="toggleRelay('fan')"></button>
                </div>

                <div class="row">
                    <div class="dev" id="devYard">
                        <div class="ico"><svg class="i"><use href="#i-bulb"/></svg></div>
                        <div><b>چراغ حیاط</b><small id="yardState">نامشخص</small></div>
                    </div>
                    <button id="swYard" class="switch" role="switch" aria-checked="false" aria-label="چراغ حیاط" onclick="toggleRelay('yardLight')"></button>
                </div>

                <div class="row">
                    <div class="dev" id="devHeat">
                        <div class="ico"><svg class="i"><use href="#i-heat"/></svg></div>
                        <div><b>گرمایش</b><small id="heatingState">نامشخص</small></div>
                    </div>
                    <button id="swHeat" class="switch" role="switch" aria-checked="false" aria-label="گرمایش" onclick="toggleRelay('heating')"></button>
                </div>

                <div class="row">
                    <div class="dev">
                        <div class="ico"><svg class="i"><use href="#i-gas"/></svg></div>
                        <div><b>خروجی شبیه‌ساز شیر گاز</b><small>کنترل مستقل منطق ایمنی</small></div>
                    </div>
                </div>

                <div id="controlMessage" class="message"></div>
            </div>

        </section>

        <!-- Security -->
        <section id="security" class="section">

            <div class="panel">
                <h2>حالت امنیتی</h2>
                <p class="lead">با توجه به حضور یا عدم حضور در ساختمان، یکی از حالت‌های زیر را انتخاب کن.</p>

                <div class="seg">
                    <button id="secMode0" onclick="setSecurityMode(0)">غیرفعال</button>
                    <button id="secMode1" onclick="setSecurityMode(1)">حالت خانه</button>
                    <button id="secMode2" onclick="setSecurityMode(2)">بیرون از خانه</button>
                </div>

                <div class="security-mode-info">

                    <div class="mode-card">
                        <span class="mode-label">حالت ۱</span>
                        <h3>غیرفعال</h3>
                        <p>
                            پایش امنیتی درب‌ها و سنسور حرکت غیرفعال است.
                            باز شدن درب‌ها یا تشخیص حرکت باعث هشدار امنیتی نمی‌شود.
                        </p>
                    </div>

                    <div class="mode-card">
                        <span class="mode-label">حالت ۲</span>
                        <h3>حالت خانه</h3>
                        <p>
                            سنسورهای درب اصلی و پارکینگ فعال هستند.
                            سنسور حرکت برای جلوگیری از هشدار هنگام حضور ساکنان غیرفعال می‌ماند.
                        </p>
                    </div>

                    <div class="mode-card">
                        <span class="mode-label">حالت ۳</span>
                        <h3>بیرون از خانه</h3>
                        <p>
                            تمام سنسورهای امنیتی، شامل درب‌ها و سنسور حرکت، فعال هستند.
                            تشخیص حرکت یا باز شدن درب می‌تواند هشدار امنیتی ایجاد کند.
                        </p>
                    </div>

                </div>

                <div class="notice" style="margin-top:16px">
                    <svg class="i"><use href="#i-info"/></svg>
                    <span>
                        اگر درب در حالت خانه یا بیرون از خانه باز بماند،
                        پس از ۱۰ ثانیه هشدار بازماندن درب فعال می‌شود.
                        هشدار صوتی کوتاه نیز هر ۳۰ ثانیه تا زمان بسته شدن درب تکرار خواهد شد.
                    </span>
                </div>

                <div class="row" style="margin-top:18px">
                    <span>وضعیت هشدار</span>
                    <strong id="securityPageAlert">--</strong>
                </div>

                <div class="actions">
                    <button class="btn danger" onclick="resetSecurity()">بازنشانی هشدار</button>
                </div>

                <p class="lead" style="margin-top:12px;margin-bottom:0">
                    بازنشانی، هشدار فعال را پاک می‌کند؛ اما حالت امنیتی انتخاب‌شده را تغییر نمی‌دهد.
                </p>

                <div id="securityMessage" class="message"></div>
            </div>

        </section>

        <!-- Automation -->
        <section id="automation" class="section">

            <div class="panel">
                <h2>حالت هوشمند</h2>
                <p class="lead">
                    با فعال‌سازی این حالت، کنترل دستی رله‌های فن،
                    چراغ حیاط و گرمایش غیرفعال می‌شود.
                </p>

                <div class="seg">
                    <button id="smartOn" onclick="setSmartMode(true)">فعال‌سازی</button>
                    <button id="smartOff" onclick="setSmartMode(false)">غیرفعال‌سازی</button>
                </div>

                <div id="automationMessage" class="message"></div>
            </div>

        </section>

        <!-- Access management -->
        <section id="access" class="section">
            <div class="panel">
                <h2>تغییر رمز مدیر</h2>
                <p class="lead">بعد از تغییر رمز، دستگاه‌های دیگر از حساب خارج می‌شوند.</p>
                <label for="currentPassword">رمز فعلی</label>
                <input type="password" id="currentPassword" autocomplete="current-password">
                <label for="newPassword">رمز جدید (حداقل ۱۰ کاراکتر)</label>
                <input type="password" id="newPassword" autocomplete="new-password">
                <label for="newPasswordConfirmation">تکرار رمز جدید</label>
                <input type="password" id="newPasswordConfirmation" autocomplete="new-password">
                <div class="actions">
                    <button class="btn primary" onclick="changePassword()">تغییر رمز</button>
                </div>
                <div id="passwordMessage" class="message"></div>
            </div>

            <div class="panel">
                <h2>دستگاه‌های واردشده</h2>
                <p class="lead">نشست‌های فعال روی این دستگاه</p>
                <div class="actions" style="margin-top:0">
                    <button class="btn" onclick="loadSessions()">به‌روزرسانی</button>
                    <button class="btn danger" onclick="logoutOtherSessions()">خروج از سایر دستگاه‌ها</button>
                </div>
                <div id="sessionsList">در حال دریافت اطلاعات...</div>
                <div id="sessionsMessage" class="message"></div>
            </div>
        </section>

        <!-- History -->
        <section id="history" class="section">

            <div class="panel">
                <h2>آخرین رویدادها</h2>
                <p class="lead">رویدادها بر اساس زمان کارکرد دستگاه ثبت می‌شوند.</p>

                <div class="actions" style="margin-top:0">
                    <button class="btn" onclick="loadHistory()">به‌روزرسانی تاریخچه</button>
                </div>

                <div id="historyList">هنوز رویدادی دریافت نشده است.</div>
            </div>

        </section>

        <!-- About project -->
        <section id="about" class="section">

            <div class="panel">
                <div class="about-intro">
                    <h2>درباره پروژه خانه هوشمند</h2>
                    <p>
                        این پروژه با استفاده از برد ESP32 طراحی شده است
                        و امکان پایش وضعیت محیط، کنترل تجهیزات، اجرای
                        اتوماسیون و مدیریت امنیت ساختمان را از طریق یک
                        داشبورد تحت وب و به‌صورت محلی فراهم می‌کند.
                    </p>
                </div>

                <div class="about-grid">

                    <div class="about-card">
                        <h3>
                            <svg class="i"><use href="#i-temp"/></svg>
                            پایش شرایط محیطی
                        </h3>
                        <ul>
                            <li>اندازه‌گیری دمای محیط با سنسور DHT22</li>
                            <li>اندازه‌گیری رطوبت محیط</li>
                            <li>تشخیص گاز با سنسور MQ-2</li>
                            <li>تشخیص بارندگی و میزان رطوبت با سنسور باران</li>
                            <li>تشخیص روشنایی محیط با سنسور LDR</li>
                        </ul>
                    </div>

                    <div class="about-card">
                        <h3>
                            <svg class="i"><use href="#i-door"/></svg>
                            پایش درب‌ها و حرکت
                        </h3>
                        <ul>
                            <li>بررسی وضعیت باز یا بسته بودن درب اصلی</li>
                            <li>بررسی وضعیت درب پارکینگ</li>
                            <li>تشخیص حرکت با سنسور PIR</li>
                            <li>نمایش وضعیت آماده‌سازی و خطای سنسورها</li>
                        </ul>
                    </div>

                    <div class="about-card">
                        <h3>
                            <svg class="i"><use href="#i-bulb"/></svg>
                            کنترل تجهیزات
                        </h3>
                        <ul>
                            <li>کنترل روشن و خاموش شدن فن</li>
                            <li>کنترل چراغ‌های حیاط</li>
                            <li>کنترل خروجی شبیه‌ساز سیستم گرمایش</li>
                            <li>نمایش وضعیت خروجی شبیه‌ساز شیر گاز</li>
                        </ul>
                    </div>

                    <div class="about-card">
                        <h3>
                            <svg class="i"><use href="#i-auto"/></svg>
                            سیستم اتوماسیون
                        </h3>
                        <ul>
                            <li>کنترل خودکار فن بر اساس دمای محیط</li>
                            <li>کنترل خودکار گرمایش بر اساس دمای محیط</li>
                            <li>کنترل چراغ حیاط بر اساس روشنایی روز و شب</li>
                            <li>غیرفعال شدن کنترل دستی رله‌ها در حالت هوشمند</li>
                        </ul>
                    </div>

                    <div class="about-card">
                        <h3>
                            <svg class="i"><use href="#i-shield"/></svg>
                            سیستم امنیتی
                        </h3>
                        <ul>
                            <li>حالت غیرفعال برای توقف پایش امنیتی</li>
                            <li>حالت خانه برای پایش درب‌ها</li>
                            <li>حالت بیرون از خانه برای پایش درب‌ها و حرکت</li>
                            <li>هشدار باز ماندن درب پس از ۱۰ ثانیه</li>
                            <li>هشدار صوتی کوتاه و تکرار آن هر ۳۰ ثانیه</li>
                            <li>بازنشانی دستی هشدار امنیتی</li>
                        </ul>
                    </div>

                    <div class="about-card">
                        <h3>
                            <svg class="i"><use href="#i-gas"/></svg>
                            ایمنی گاز
                        </h3>
                        <ul>
                            <li>پایش وضعیت گاز با سنسور MQ-2</li>
                            <li>نمایش وضعیت آماده‌سازی سنسور پس از روشن شدن</li>
                            <li>فعال شدن منطق ایمنی هنگام تشخیص گاز</li>
                            <li>قفل شدن خروجی شبیه‌ساز شیر گاز تا بازگشایی مجاز</li>
                        </ul>
                    </div>

                    <div class="about-card">
                        <h3>
                            <svg class="i"><use href="#i-lock"/></svg>
                            امنیت دسترسی
                        </h3>
                        <ul>
                            <li>تعریف رمز عبور مدیر در راه‌اندازی اولیه</li>
                            <li>ورود به داشبورد با رمز مدیر</li>
                            <li>تغییر رمز عبور مدیر</li>
                            <li>مشاهده نشست‌های فعال</li>
                            <li>خروج از سایر دستگاه‌ها</li>
                        </ul>
                    </div>

                    <div class="about-card">
                        <h3>
                            <svg class="i"><use href="#i-history"/></svg>
                            تاریخچه و پایش سیستم
                        </h3>
                        <ul>
                            <li>ثبت رویدادهای مهم سیستم</li>
                            <li>نمایش زمان سپری‌شده از شروع به کار دستگاه</li>
                            <li>به‌روزرسانی خودکار اطلاعات داشبورد</li>
                            <li>نمایش وضعیت اتصال به ESP32</li>
                        </ul>
                    </div>

                </div>

                <div class="about-note">
                    نکته مهم: داشبورد از طریق شبکه محلی دستگاه در دسترس است.
                    خروجی‌های مربوط به شیر گاز و گرمایش در این نسخه شبیه‌ساز هستند
                    و به معنی وجود شیر برقی یا سیستم گرمایشی واقعی نیستند.
                </div>

            </div>

            <div class="panel">
                <h2>بستر فنی پروژه</h2>
                <p class="lead">
                    پردازش اطلاعات سنسورها و اجرای فرمان‌ها توسط برد ESP32 انجام می‌شود.
                </p>

                <div class="row">
                    <span>واحد پردازش مرکزی</span>
                    <strong>ESP32</strong>
                </div>

                <div class="row">
                    <span>رابط کاربری</span>
                    <strong>داشبورد تحت وب</strong>
                </div>

                <div class="row">
                    <span>روش ارتباط</span>
                    <strong>شبکه محلی Wi-Fi</strong>
                </div>

                <div class="row">
                    <span>روش کنترل</span>
                    <strong>خودکار و دستی</strong>
                </div>
            </div>

        </section>

        </div>

    </main>

</div>

<footer>Smart Home - Local Dashboard</footer>

<script>

let token = sessionStorage.getItem("smartHomeToken") || "";
let refreshTimer = null;
let relayState = { fan: false, yardLight: false, heating: false };

const $ = id => document.getElementById(id);

/* ---------- Theme ---------- */

function applyTheme(theme) {
    if (theme) {
        document.documentElement.setAttribute("data-theme", theme);
    }
    const dark = theme
        ? theme === "dark"
        : window.matchMedia("(prefers-color-scheme: dark)").matches;
    $("themeIcon").setAttribute("href", dark ? "#i-sun" : "#i-moon");
}

function toggleTheme() {
    const current = document.documentElement.getAttribute("data-theme");
    const dark = current
        ? current === "dark"
        : window.matchMedia("(prefers-color-scheme: dark)").matches;
    const next = dark ? "light" : "dark";
    applyTheme(next);
    try { localStorage.setItem("smartHomeTheme", next); } catch (e) {}
}

(function () {
    let saved = null;
    try { saved = localStorage.getItem("smartHomeTheme"); } catch (e) {}
    applyTheme(saved === "dark" || saved === "light" ? saved : null);
})();

/* ---------- API ---------- */

async function api(path, method = "GET", body = null) {

    const headers = {
        "Content-Type": "application/json"
    };

    if (token) {
        headers["Authorization"] = "Bearer " + token;
    }

    const options = {
        method,
        headers
    };

    if (body !== null) {
        options.body = JSON.stringify(body);
    }

    const response = await fetch(path, options);

    let data = {};

    try {
        data = await response.json();
    } catch (error) {
        throw new Error("پاسخ نامعتبر از دستگاه");
    }

    if (response.status === 401) {
        logout(false);
        throw new Error("نشست شما منقضی شده است");
    }

    if (!response.ok) {
        throw new Error(data.message || "خطا در اجرای درخواست");
    }

    return data;
}

async function login() {

    const password = $("password").value;

    if (!password) {
        $("loginMessage").textContent = "رمز عبور را وارد کن";
        return;
    }

    try {

        const result = await api(
            "/api/login",
            "POST",
            {
                password,
                device: navigator.userAgent.substring(0,100)
            }
        );

        token = result.token;

        sessionStorage.setItem("smartHomeToken", token);

        $("password").value = "";
        $("loginMessage").textContent = "";

        showDashboard();

    } catch (error) {

        $("loginMessage").textContent = error.message;
    }
}

function showDashboard() {

    $("setupScreen").classList.add("hidden");
    $("loginScreen").classList.add("hidden");
    $("dashboard").classList.remove("hidden");
    $("logoutButton").classList.remove("hidden");

    refreshStatus();

    loadHistory();
    loadSessions();

    if (refreshTimer) {
        clearInterval(refreshTimer);
    }

    refreshTimer = setInterval(refreshStatus, 1000);
}

async function logout(sendRequest = true) {

    if (sendRequest && token) {
        try {
            await api("/api/logout", "POST");
        } catch (error) {
            // اگر ارتباط قطع باشد، توکن محلی پاک می‌شود؛ نشست سرور تا انقضا باقی می‌ماند.
        }
    }

    token = "";
    sessionStorage.removeItem("smartHomeToken");

    if (refreshTimer) {
        clearInterval(refreshTimer);
        refreshTimer = null;
    }

    $("dashboard").classList.add("hidden");
    $("setupScreen").classList.add("hidden");
    $("loginScreen").classList.remove("hidden");
    $("logoutButton").classList.add("hidden");

    $("connectionStatus").textContent = "خارج شده";
    $("connectionStatus").className = "status warning";
}

/* ---------- Helpers ---------- */

function setConnection(connected) {

    $("connectionStatus").textContent =
        connected ? "متصل" : "قطع ارتباط";

    $("connectionStatus").className =
        connected ? "status good" : "status bad";
}

function sensorStatus(value) {

    const labels = [
        "در حال آماده‌سازی",
        "آماده",
        "خطا",
        "قطع ارتباط"
    ];

    return labels[value] || "نامشخص";
}

function setStatus(id, value) {

    $(id).textContent = value;
}

function setChip(id, value) {

    $(id).textContent = sensorStatus(value);
    $(id).className = "status " + (["warning", "good", "bad", "bad"][value] || "");
}

function cardTone(id, tone) {

    const card = $(id).closest(".card");

    if (!card) return;

    if (tone) {
        card.setAttribute("data-tone", tone);
    } else {
        card.removeAttribute("data-tone");
    }
}

function formatUptime(seconds) {

    const h = Math.floor(seconds / 3600);
    const m = Math.floor((seconds % 3600) / 60);
    const s = seconds % 60;

    return `${h} ساعت ${m} دقیقه ${s} ثانیه`;
}

function setSwitch(buttonId, deviceId, on) {

    $(buttonId).setAttribute("aria-checked", on ? "true" : "false");
    $(deviceId).classList.toggle("on", !!on);
}

function setSegment(ids, activeId) {

    ids.forEach(id => $(id).classList.toggle("on", id === activeId));
}

function updateHero(data) {

    const s = data.sensors;
    const issues = [];
    let tone = "good";
    let title = "همه‌چیز عادی است";
    let icon = "#i-check";

    if (data.gasAlert) issues.push("هشدار گاز فعال است");
    if (data.securityAlert) issues.push("هشدار امنیتی فعال است");

    const faults = [s.dhtStatus, s.mq2Status, s.pirStatus, s.rainStatus, s.ldrStatus]
        .filter(v => v === 2 || v === 3).length;

    if (issues.length) {
        tone = "bad";
        title = issues.join(" و ");
        icon = "#i-alert";
    } else if (faults) {
        tone = "warn";
        title = "بعضی سنسورها مشکل دارند";
        icon = "#i-alert";
    }

    const notes = [];

    if (faults && !issues.length) {
        notes.push(faults + " سنسور خطا یا قطع ارتباط دارد");
    }
    if (s.dhtStatus === 1) {
        notes.push("دما " + s.temperature.toFixed(1) + " درجه و رطوبت " + s.humidity.toFixed(0) + " درصد");
    }
    if (s.mainDoorOpen) notes.push("درب اصلی باز است");
    if (s.garageDoorOpen) notes.push("درب پارکینگ باز است");
    if (s.rainStatus === 1 && s.rainDetected) notes.push("باران یا رطوبت تشخیص داده شده");

    $("hero").setAttribute("data-tone", tone === "good" ? "good" : tone);
    $("heroIcon").setAttribute("href", icon);
    $("heroTitle").textContent = title;
    $("heroSub").textContent = notes.join(" — ") || "اطلاعات سنسورها در دسترس نیست";
}

/* ---------- Trend charts ---------- */

const CHART_KEEP = 1800;           // نگهداری ۳۰ دقیقه داده در مرورگر
let chartRange = 300;              // بازه نمایش فعلی (ثانیه)
let samples = [];

const CHARTS = [
    { key: "temp", box: "chartTemp", now: "chNowTemp", stats: "statsTemp", unit: " °C", dec: 1, minSpan: 4, def: [20, 30] },
    { key: "hum",  box: "chartHum",  now: "chNowHum",  stats: "statsHum",  unit: " %",  dec: 1, minSpan: 10, def: [30, 70], clamp: [0, 100] },
    { key: "rain", box: "chartRain", now: "chNowRain", stats: "statsRain", unit: " %",  dec: 0, fixed: [0, 100] }
];

(function restoreSamples() {
    try {
        const raw = JSON.parse(sessionStorage.getItem("smartHomeSamples") || "[]");
        const cut = Date.now() - CHART_KEEP * 1000;
        samples = raw
            .filter(a => a[0] >= cut)
            .map(a => ({ t: a[0], temp: a[1], hum: a[2], rain: a[3], wet: !!a[4] }));
    } catch (e) { samples = []; }
})();

window.addEventListener("pagehide", () => {
    try {
        sessionStorage.setItem("smartHomeSamples", JSON.stringify(
            samples.map(s => [s.t, s.temp, s.hum, s.rain, s.wet ? 1 : 0])
        ));
    } catch (e) {}
});

function num(v) { return typeof v === "number" && isFinite(v) ? v : null; }

function pushSample(s) {

    const now = Date.now();
    const ok = s.dhtStatus === 1;
    const rainOk = s.rainStatus === 1;

    samples.push({
        t: now,
        temp: ok ? num(s.temperature) : null,
        hum: ok ? num(s.humidity) : null,
        rain: rainOk ? num(s.rainPercent) : null,
        wet: rainOk && !!s.rainDetected
    });

    const cut = now - CHART_KEEP * 1000;
    while (samples.length && samples[0].t < cut) samples.shift();

    CHARTS.forEach(drawChart);
}

function setChartRange(sec, button) {

    chartRange = sec;

    button.parentNode.querySelectorAll("button").forEach(b => b.classList.toggle("on", b === button));

    CHARTS.forEach(drawChart);
}

function niceNum(r, round) {
    const e = Math.floor(Math.log10(r));
    const f = r / Math.pow(10, e);
    let n;
    if (round) n = f < 1.5 ? 1 : f < 3 ? 2 : f < 7 ? 5 : 10;
    else n = f <= 1 ? 1 : f <= 2 ? 2 : f <= 5 ? 5 : 10;
    return n * Math.pow(10, e);
}

function niceScale(lo, hi, ticks) {
    const range = niceNum(hi - lo, false);
    const step = niceNum(range / (ticks - 1), true);
    return { min: Math.floor(lo / step) * step, max: Math.ceil(hi / step) * step, step };
}

function clock(t, withSeconds) {
    const d = new Date(t);
    const p = n => String(n).padStart(2, "0");
    return p(d.getHours()) + ":" + p(d.getMinutes()) + (withSeconds ? ":" + p(d.getSeconds()) : "");
}

// منحنی نرم بدون بالازدگی (monotone cubic)
function curve(p) {
    const n = p.length;
    let d = "M" + p[0].x.toFixed(1) + " " + p[0].y.toFixed(1);
    if (n === 1) return d;
    if (n === 2) return d + "L" + p[1].x.toFixed(1) + " " + p[1].y.toFixed(1);

    const dx = [], m = [], t = [];
    for (let i = 0; i < n - 1; i++) {
        dx[i] = p[i + 1].x - p[i].x || 1;
        m[i] = (p[i + 1].y - p[i].y) / dx[i];
    }
    t[0] = m[0];
    t[n - 1] = m[n - 2];
    for (let i = 1; i < n - 1; i++) {
        t[i] = m[i - 1] * m[i] <= 0 ? 0 : (m[i - 1] + m[i]) / 2;
    }
    for (let i = 0; i < n - 1; i++) {
        if (m[i] === 0) { t[i] = 0; t[i + 1] = 0; continue; }
        const a = t[i] / m[i], b = t[i + 1] / m[i], s = a * a + b * b;
        if (s > 9) {
            const k = 3 / Math.sqrt(s);
            t[i] = k * a * m[i];
            t[i + 1] = k * b * m[i];
        }
    }
    for (let i = 0; i < n - 1; i++) {
        const c1x = p[i].x + dx[i] / 3, c1y = p[i].y + t[i] * dx[i] / 3;
        const c2x = p[i + 1].x - dx[i] / 3, c2y = p[i + 1].y - t[i + 1] * dx[i] / 3;
        d += "C" + c1x.toFixed(1) + " " + c1y.toFixed(1) + " " + c2x.toFixed(1) + " " + c2y.toFixed(1) +
             " " + p[i + 1].x.toFixed(1) + " " + p[i + 1].y.toFixed(1);
    }
    return d;
}

function collect(c, t0, N) {

    const raw = samples.filter(s => s.t >= t0 && s[c.key] !== null && s[c.key] !== undefined);

    let mn = Infinity, mx = -Infinity, sum = 0, wet = 0;

    raw.forEach(s => {
        const v = s[c.key];
        if (v < mn) mn = v;
        if (v > mx) mx = v;
        sum += v;
        if (s.wet) wet++;
    });

    let pts = [];
    const bucketMs = chartRange * 1000 / N;

    if (raw.length <= N) {
        pts = raw.map(s => ({ t: s.t, v: s[c.key], wet: s.wet }));
    } else {
        let cur = null, idx = -1;
        const flush = () => { if (cur) pts.push({ t: cur.st / cur.n, v: cur.sv / cur.n, wet: cur.wet }); };
        raw.forEach(s => {
            const i = Math.floor(s.t / bucketMs);
            if (i !== idx) { flush(); cur = { sv: 0, st: 0, n: 0, wet: false }; idx = i; }
            cur.sv += s[c.key];
            cur.st += s.t;
            cur.n++;
            if (s.wet) cur.wet = true;
        });
        flush();
    }

    return {
        pts,
        n: raw.length,
        mn, mx,
        avg: raw.length ? sum / raw.length : 0,
        wetPct: raw.length ? wet / raw.length * 100 : 0,
        bucketMs
    };
}

function drawChart(c) {

    const box = $(c.box);

    if (!box || !box.offsetParent) return;

    if (!c.svg) bindChart(c);

    const w = Math.max(240, Math.round(box.clientWidth));
    const h = 176;
    const L = 40, R = 12, T = 12, B = 24;
    const pw = w - L - R, ph = h - T - B;

    const now = Date.now();
    const W = chartRange * 1000;
    const t0 = now - W;

    const data = collect(c, t0, Math.max(40, Math.floor(pw / 3)));
    const pts = data.pts;

    // مقدار فعلی و آمار بازه
    let last = null;
    for (let i = samples.length - 1; i >= 0; i--) {
        if (samples[i][c.key] !== null && samples[i][c.key] !== undefined) { last = samples[i]; break; }
    }

    $(c.now).textContent = last ? last[c.key].toFixed(c.dec) + c.unit : "--";

    if (c.key === "rain") {
        $("chStateRain").textContent = last ? (last.wet ? "بارندگی یا رطوبت تشخیص داده شد" : "خشک") : "";
    }

    const f = v => v.toFixed(c.dec);
    let st = "";
    if (data.n) {
        st += "<div><small>کمترین</small><b>" + f(data.mn) + c.unit + "</b></div>";
        st += "<div><small>بیشترین</small><b>" + f(data.mx) + c.unit + "</b></div>";
        st += "<div><small>میانگین</small><b>" + f(data.avg) + c.unit + "</b></div>";
        if (c.key === "rain") {
            st += "<div><small>زمان بارندگی</small><b>" + data.wetPct.toFixed(0) + " %</b></div>";
        }
    }
    $(c.stats).innerHTML = st;

    if (c.hover) return;

    // مقیاس محور عمودی
    let sc;
    if (c.fixed) {
        sc = { min: c.fixed[0], max: c.fixed[1], step: 25 };
    } else {
        let lo = c.def[0], hi = c.def[1];
        if (pts.length) {
            lo = Infinity; hi = -Infinity;
            pts.forEach(p => { if (p.v < lo) lo = p.v; if (p.v > hi) hi = p.v; });
            if (hi - lo < c.minSpan) {
                const m = (hi + lo) / 2;
                lo = m - c.minSpan / 2;
                hi = m + c.minSpan / 2;
            } else {
                const pad = (hi - lo) * 0.12;
                lo -= pad; hi += pad;
            }
        }
        sc = niceScale(lo, hi, 5);
        if (c.clamp) {
            sc.min = Math.max(c.clamp[0], sc.min);
            sc.max = Math.min(c.clamp[1], sc.max);
        }
    }

    const X = t => L + (t - t0) / W * pw;
    const Y = v => T + ph - (v - sc.min) / (sc.max - sc.min) * ph;
    const dec = sc.step < 1 ? 1 : 0;

    let s = '<defs><linearGradient id="g_' + c.key + '" x1="0" y1="0" x2="0" y2="1">' +
            '<stop offset="0" style="stop-color:var(--c);stop-opacity:.30"/>' +
            '<stop offset="1" style="stop-color:var(--c);stop-opacity:0"/></linearGradient></defs>';

    // خطوط راهنما و برچسب‌های عمودی
    for (let v = sc.min; v <= sc.max + 1e-9; v += sc.step) {
        const y = Y(v).toFixed(1);
        s += '<line class="gl' + (v === sc.min ? ' base' : '') + '" x1="' + L + '" x2="' + (w - R) + '" y1="' + y + '" y2="' + y + '"/>';
        s += '<text class="ax" x="' + (L - 7) + '" y="' + (+y + 4) + '" text-anchor="end">' + v.toFixed(dec) + '</text>';
    }

    // برچسب‌های زمان
    for (let i = 0; i <= 3; i++) {
        const x = L + pw * i / 3;
        const anchor = i === 0 ? "start" : i === 3 ? "end" : "middle";
        s += '<text class="ax" x="' + x.toFixed(1) + '" y="' + (h - 5) + '" text-anchor="' + anchor + '">' +
             clock(t0 + W * i / 3, chartRange <= 300) + '</text>';
    }

    const gap = Math.max(8000, data.bucketMs * 3);

    // نوارهای بارندگی
    if (c.key === "rain") {
        pts.forEach((p, i) => {
            if (!p.wet) return;
            const a = i > 0 && p.t - pts[i - 1].t <= gap ? (X(pts[i - 1].t) + X(p.t)) / 2 : X(p.t) - 2;
            const b = i < pts.length - 1 && pts[i + 1].t - p.t <= gap ? (X(p.t) + X(pts[i + 1].t)) / 2 : X(p.t) + 2;
            s += '<rect class="band" x="' + a.toFixed(1) + '" y="' + T + '" width="' + Math.max(2, b - a + 0.6).toFixed(1) + '" height="' + ph + '"/>';
        });
    }

    // بخش‌های پیوسته داده
    const geo = pts.map(p => ({ t: p.t, v: p.v, wet: p.wet, px: X(p.t), py: Y(p.v) }));
    const segs = [];
    geo.forEach((p, i) => {
        if (i === 0 || p.t - geo[i - 1].t > gap) segs.push([]);
        segs[segs.length - 1].push({ x: p.px, y: p.py });
    });

    const base = (T + ph).toFixed(1);

    segs.forEach(seg => {
        if (seg.length < 2) return;
        const d = curve(seg);
        s += '<path d="' + d + 'L' + seg[seg.length - 1].x.toFixed(1) + ' ' + base +
             'L' + seg[0].x.toFixed(1) + ' ' + base + 'Z" fill="url(#g_' + c.key + ')"/>';
        s += '<path class="ln" d="' + d + '"/>';
    });

    if (geo.length) {
        const e = geo[geo.length - 1];
        s += '<circle class="ring" cx="' + e.px.toFixed(1) + '" cy="' + e.py.toFixed(1) + '" r="5"/>';
        s += '<circle class="dot" cx="' + e.px.toFixed(1) + '" cy="' + e.py.toFixed(1) + '" r="4.5"/>';
    }

    if (geo.length < 2) {
        s += '<text class="ax" x="' + (L + pw / 2) + '" y="' + (T + ph / 2) + '" text-anchor="middle">در حال جمع‌آوری داده…</text>';
    }

    s += '<g class="hv" style="display:none"><line y1="' + T + '" y2="' + (T + ph) + '"/>' +
         '<circle class="dot" r="5"/></g>';

    c.svg.setAttribute("viewBox", "0 0 " + w + " " + h);
    c.svg.innerHTML = s;
    c.geo = { pts: geo, w, h, T, ph };
}

function bindChart(c) {

    const box = $(c.box);

    c.svg = box.querySelector("svg");
    c.tip = box.querySelector(".chart-tip");
    c.hover = false;
    c.geo = null;

    const move = e => {

        if (!c.geo || !c.geo.pts.length) return;

        const r = c.svg.getBoundingClientRect();
        const k = c.geo.w / r.width;
        const x = (e.clientX - r.left) * k;

        let best = c.geo.pts[0];
        c.geo.pts.forEach(p => { if (Math.abs(p.px - x) < Math.abs(best.px - x)) best = p; });

        c.hover = true;

        const hv = c.svg.querySelector(".hv");
        if (!hv) return;

        hv.style.display = "";
        const line = hv.querySelector("line");
        line.setAttribute("x1", best.px);
        line.setAttribute("x2", best.px);
        const dot = hv.querySelector("circle");
        dot.setAttribute("cx", best.px);
        dot.setAttribute("cy", best.py);

        c.tip.innerHTML = "<span>" + clock(best.t, true) + "</span><b>" + best.v.toFixed(c.dec) + c.unit + "</b>" +
            (c.key === "rain" && best.wet ? "<span>بارندگی</span>" : "");
        c.tip.hidden = false;

        const tw = c.tip.offsetWidth;
        const left = Math.min(Math.max(best.px / k - tw / 2, 0), r.width - tw);
        c.tip.style.left = left + "px";
    };

    const leave = () => {
        c.hover = false;
        c.tip.hidden = true;
        drawChart(c);
    };

    c.svg.addEventListener("pointermove", move);
    c.svg.addEventListener("pointerdown", move);
    c.svg.addEventListener("pointerleave", leave);
    c.svg.addEventListener("pointercancel", leave);
}

window.addEventListener("resize", () => CHARTS.forEach(drawChart));

/* ---------- Status refresh ---------- */

async function refreshStatus() {

    try {

        const data = await api("/api/status");

        const s = data.sensors;
        const r = data.relays;

        setConnection(true);

        setStatus(
            "temperature",
            s.dhtStatus === 1
                ? s.temperature.toFixed(1) + " °C"
                : "--"
        );

        setStatus(
            "humidity",
            s.dhtStatus === 1
                ? s.humidity.toFixed(1) + " %"
                : "--"
        );

        setChip("dhtStatus", s.dhtStatus);
        setChip("mq2Status", s.mq2Status);
        setChip("pirStatus", s.pirStatus);
        setChip("rainStatus", s.rainStatus);
        setChip("ldrStatus", s.ldrStatus);

        setStatus(
            "gas",
            s.mq2Status === 1
                ? (s.gasDetected ? "تشخیص گاز" : "بدون تشخیص گاز")
                : "در دسترس نیست"
        );
        cardTone("gas", s.mq2Status === 1 && s.gasDetected ? "bad" : (s.mq2Status === 1 ? "good" : ""));

        setStatus(
            "pir",
            s.pirStatus === 1
                ? (s.pirMotion ? "حرکت تشخیص داده شد" : "بدون حرکت")
                : "در حال آماده‌سازی"
        );
        cardTone("pir", s.pirStatus === 1 && s.pirMotion ? "warn" : "");

        setStatus(
            "mainDoor",
            s.mainDoorOpen ? "باز" : "بسته"
        );
        cardTone("mainDoor", s.mainDoorOpen ? "warn" : "good");

        setStatus(
            "garageDoor",
            s.garageDoorOpen ? "باز" : "بسته"
        );
        cardTone("garageDoor", s.garageDoorOpen ? "warn" : "good");

        setStatus(
            "rain",
            s.rainStatus === 1
                ? (s.rainDetected ? ("رطوبت/بارندگی — " + s.rainPercent + "%") : "خشک — " + s.rainPercent + "%")
                : "در حال آماده‌سازی"
        );
        cardTone("rain", s.rainStatus === 1 && s.rainDetected ? "warn" : "");

        // مقدار دیجیتال LDR: صفر = روز، یک = شب
        setStatus(
            "ldr",
            s.ldrStatus === 1
                ? (s.ldrState ? "شب" : "روز")
                : "نامشخص"
        );

        setStatus(
            "smartStatus",
            data.smartMode ? "فعال" : "غیرفعال"
        );

        const modes = [
            "غیرفعال",
            "حالت خانه",
            "حالت بیرون از خانه"
        ];

        setStatus(
            "securityStatus",
            modes[data.securityMode] || "نامشخص"
        );

        setStatus(
            "securityAlert",
            data.securityAlert ? "فعال" : "غیرفعال"
        );

        setStatus(
            "securityPageAlert",
            data.securityAlert ? "هشدار فعال" : "بدون هشدار"
        );

        setStatus(
            "gasLock",
            data.gasValveLocked ? "قفل و بسته" : "قفل نیست"
        );

        setStatus("gasAlertStatus", data.gasAlert ? "هشدار فعال" : "بدون هشدار");
        setStatus("gasValveOutput", r.gasValve ? "فعال / بازنمایی باز" : "غیرفعال / بازنمایی بسته");
        setStatus("fanState", r.fan ? "روشن" : "خاموش");
        setStatus("yardState", r.yardLight ? "روشن" : "خاموش");
        setStatus("heatingState", r.heating ? "روشن" : "خاموش");

        relayState.fan = !!r.fan;
        relayState.yardLight = !!r.yardLight;
        relayState.heating = !!r.heating;

        setSwitch("swFan", "devFan", r.fan);
        setSwitch("swYard", "devYard", r.yardLight);
        setSwitch("swHeat", "devHeat", r.heating);

        setStatus(
            "uptime",
            formatUptime(data.uptimeSeconds)
        );

        // خلاصه بالای صفحه و دکمه‌های انتخاب حالت
        $("heroSec").textContent = modes[data.securityMode] || "نامشخص";
        $("heroSmart").textContent = data.smartMode ? "فعال" : "غیرفعال";
        updateHero(data);
        pushSample(s);

        setSegment(["secMode0", "secMode1", "secMode2"], "secMode" + data.securityMode);
        setSegment(["smartOn", "smartOff"], data.smartMode ? "smartOn" : "smartOff");

        $("smartNotice").classList.toggle("active", !!data.smartMode);

        document.querySelectorAll("#controls button").forEach(button => {
            button.disabled = data.smartMode;
        });

    } catch (error) {

        setConnection(false);
    }
}

/* ---------- Actions ---------- */

function toggleRelay(name) {

    setRelay(name, !relayState[name]);
}

async function setRelay(name, enabled) {

    try {

        await api(
            "/api/relay",
            "POST",
            { name, enabled }
        );

        $("controlMessage").textContent =
            "فرمان ارسال شد";

        refreshStatus();

    } catch (error) {

        $("controlMessage").textContent = error.message;
    }
}

async function setSecurityMode(mode) {

    try {

        await api(
            "/api/security/mode",
            "POST",
            { mode }
        );

        $("securityMessage").textContent =
            "حالت امنیتی تغییر کرد";

        refreshStatus();

    } catch (error) {

        $("securityMessage").textContent = error.message;
    }
}

async function resetSecurity() {

    if (!confirm("هشدار امنیتی بازنشانی شود؟")) {
        return;
    }

    try {

        await api("/api/security/reset", "POST");

        $("securityMessage").textContent =
            "درخواست بازنشانی ارسال شد";

        refreshStatus();

    } catch (error) {

        $("securityMessage").textContent = error.message;
    }
}

async function setSmartMode(enabled) {

    try {

        await api(
            "/api/automation/smart-mode",
            "POST",
            { enabled }
        );

        $("automationMessage").textContent =
            "تنظیم حالت هوشمند تغییر کرد";

        refreshStatus();

    } catch (error) {

        $("automationMessage").textContent = error.message;
    }
}

async function loadHistory() {

    try {

        const data = await api("/api/history");

        const list = $("historyList");

        list.innerHTML = "";

        if (!data.events || data.events.length === 0) {

            list.textContent = "هنوز رویدادی ثبت نشده است.";

            return;
        }

        data.events.forEach(event => {

            const item = document.createElement("div");

            item.className = "event";

            const title = document.createElement("strong");
            title.textContent = event.message;

            const detail = document.createElement("small");

            detail.textContent =
                "شناسه: " + event.id +
                " | زمان از شروع سیستم: " +
                formatUptime(event.uptimeSeconds);

            item.appendChild(title);
            item.appendChild(detail);

            if (event.details) {

                const extra = document.createElement("small");
                extra.textContent = event.details;

                item.appendChild(extra);
            }

            list.appendChild(item);
        });

    } catch (error) {

        $("historyList").textContent = error.message;
    }
}

function showSection(id, button) {

    document.querySelectorAll(".section").forEach(section => {
        section.classList.remove("active");
    });

    document.querySelectorAll(".nav button").forEach(item => {
        item.classList.remove("active");
    });

    $(id).classList.add("active");

    button.classList.add("active");

    window.scrollTo(0, 0);

    if (id === "home") {
        CHARTS.forEach(drawChart);
    }

    if (id === "history") {
        loadHistory();
    }
}

function showSetup() {
    token = "";
    sessionStorage.removeItem("smartHomeToken");
    $("setupScreen").classList.remove("hidden");
    $("loginScreen").classList.add("hidden");
    $("dashboard").classList.add("hidden");
    $("logoutButton").classList.add("hidden");
}

function showLogin() {
    $("setupScreen").classList.add("hidden");
    $("loginScreen").classList.remove("hidden");
    $("dashboard").classList.add("hidden");
    $("logoutButton").classList.add("hidden");
}

async function setupAccount() {
    const password = $("setupPassword").value;
    const confirmation = $("setupConfirmation").value;

    if (password.length < 10) {
        $("setupMessage").textContent = "رمز باید حداقل ۱۰ کاراکتر باشد";
        return;
    }
    if (password !== confirmation) {
        $("setupMessage").textContent = "تکرار رمز با رمز اصلی یکسان نیست";
        return;
    }

    try {
        await api("/api/setup", "POST", { password, confirmation });
        $("setupPassword").value = "";
        $("setupConfirmation").value = "";
        $("setupMessage").textContent = "";
        showLogin();
        $("loginMessage").textContent = "رمز مدیر ثبت شد؛ اکنون وارد شو.";
    } catch (error) {
        $("setupMessage").textContent = error.message;
    }
}

async function changePassword() {
    const currentPassword = $("currentPassword").value;
    const newPassword = $("newPassword").value;
    const confirmation = $("newPasswordConfirmation").value;

    try {
        await api("/api/auth/password", "POST", { currentPassword, newPassword, confirmation });
        $("currentPassword").value = "";
        $("newPassword").value = "";
        $("newPasswordConfirmation").value = "";
        $("passwordMessage").textContent = "رمز تغییر کرد؛ نشست‌های دیگر خارج شدند.";
        loadSessions();
    } catch (error) {
        $("passwordMessage").textContent = error.message;
    }
}

async function loadSessions() {
    try {
        const data = await api("/api/auth/sessions");
        const list = $("sessionsList");
        list.innerHTML = "";

        if (!data.sessions || data.sessions.length === 0) {
            list.textContent = "نشست فعالی وجود ندارد.";
            return;
        }

        data.sessions.forEach(session => {
            const item = document.createElement("div");
            item.className = "event";
            const title = document.createElement("strong");
            title.textContent = (session.current ? "این دستگاه — " : "دستگاه دیگر — ") +
                (session.device || "دستگاه ناشناس");
            const detail = document.createElement("small");
            detail.textContent = "آخرین فعالیت: " + session.lastActivitySecondsAgo + " ثانیه قبل";
            item.appendChild(title);
            item.appendChild(detail);
            list.appendChild(item);
        });
    } catch (error) {
        $("sessionsList").textContent = error.message;
    }
}

async function logoutOtherSessions() {
    if (!confirm("از تمام دستگاه‌های دیگر خارج شوی؟")) return;
    try {
        await api("/api/auth/logout-others", "POST");
        $("sessionsMessage").textContent = "نشست‌های دیگر لغو شدند.";
        loadSessions();
    } catch (error) {
        $("sessionsMessage").textContent = error.message;
    }
}

async function initialize() {
    try {
        const setup = await api("/api/setup-status");
        if (setup.setupRequired) {
            showSetup();
            return;
        }

        showLogin();
        if (token) {
            try {
                await api("/api/status");
                showDashboard();
            } catch (error) {
                logout(false);
                showLogin();
            }
        }
    } catch (error) {
        $("connectionStatus").textContent = "ارتباط با دستگاه برقرار نشد";
        $("connectionStatus").className = "status bad";
        showLogin();
    }
}

// ورود با کلید Enter
$("password").addEventListener("keydown", e => { if (e.key === "Enter") login(); });
$("setupConfirmation").addEventListener("keydown", e => { if (e.key === "Enter") setupAccount(); });

initialize();

</script>

</body>
</html>
)HTML";