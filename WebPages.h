#pragma once

#include <Arduino.h>

// HTML-страницы web-интерфейса (PROGMEM).
namespace WebPages {

constexpr char STYLE[] PROGMEM = R"raw(
<style>
  * { box-sizing: border-box; }
  body {
    font-family: Arial, sans-serif;
    margin: 0;
    background: #f0f4f8;
    color: #1f2937;
  }
  .wrap {
    max-width: 720px;
    margin: 40px auto;
    padding: 24px;
    background: #fff;
    border-radius: 12px;
    box-shadow: 0 8px 24px rgba(0,0,0,.08);
  }
  h1 { margin-top: 0; font-size: 1.5rem; }
  nav { margin-bottom: 20px; }
  nav a {
    margin-right: 12px;
    color: #2563eb;
    text-decoration: none;
    font-weight: 600;
  }
  label { display: block; margin: 12px 0 6px; font-weight: 600; }
  input[type=text], input[type=password] {
    width: 100%;
    padding: 10px 12px;
    border: 1px solid #cbd5e1;
    border-radius: 8px;
    font-size: 1rem;
  }
  button, .btn {
    margin-top: 12px;
    margin-right: 8px;
    padding: 10px 16px;
    border: none;
    border-radius: 8px;
    font-size: 1rem;
    cursor: pointer;
    background: #2563eb;
    color: #fff;
  }
  button.secondary { background: #64748b; }
  button.danger { background: #dc2626; }
  .status {
    margin-top: 16px;
    padding: 12px;
    border-radius: 8px;
    background: #eff6ff;
    font-weight: 600;
  }
  .error { color: #dc2626; margin-bottom: 12px; }
  canvas {
    width: 100%;
    height: 280px;
    border: 1px solid #e2e8f0;
    border-radius: 8px;
    background: #fff;
  }
</style>
)raw";

inline String loginPage(bool showError) {
  String html = F("<!DOCTYPE html><html lang=\"ru\"><head><meta charset=\"UTF-8\">");
  html += F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
  html += F("<title>Lab3 — Вход</title>");
  html += FPSTR(STYLE);
  html += F("</head><body><div class=\"wrap\">");
  html += F("<h1>Аутентификация</h1>");
  if (showError) {
    html += F("<p class=\"error\">Неверный логин или пароль</p>");
  }
  html += F("<form method=\"POST\" action=\"/login\">");
  html += F("<label for=\"username\">Логин</label>");
  html += F("<input id=\"username\" name=\"username\" type=\"text\" required>");
  html += F("<label for=\"password\">Пароль</label>");
  html += F("<input id=\"password\" name=\"password\" type=\"password\" required>");
  html += F("<button type=\"submit\">Войти</button>");
  html += F("</form></div></body></html>");
  return html;
}

inline String ledPage() {
  String html = F("<!DOCTYPE html><html lang=\"ru\"><head><meta charset=\"UTF-8\">");
  html += F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
  html += F("<title>Lab3 — Светодиод</title>");
  html += FPSTR(STYLE);
  html += F("</head><body><div class=\"wrap\">");
  html += F("<nav><a href=\"/led\">Светодиод</a><a href=\"/data\">Мониторинг</a></nav>");
  html += F("<h1>Управление светодиодом</h1>");
  html += F("<button id=\"btnOn\">Включить</button>");
  html += F("<button id=\"btnOff\" class=\"danger\">Выключить</button>");
  html += F("<div class=\"status\" id=\"status\">Состояние: загрузка...</div>");
  html += F("<script>");
  html += F("async function refresh(){");
  html += F("const r=await fetch('/api/led/state');");
  html += F("const j=await r.json();");
  html += F("document.getElementById('status').textContent='Состояние: '+(j.on?'Включен':'Выключен');");
  html += F("}");
  html += F("document.getElementById('btnOn').onclick=async()=>{await fetch('/api/led/on',{method:'POST'});refresh();};");
  html += F("document.getElementById('btnOff').onclick=async()=>{await fetch('/api/led/off',{method:'POST'});refresh();};");
  html += F("refresh();</script></div></body></html>");
  return html;
}

inline String dataPage() {
  String html = F("<!DOCTYPE html><html lang=\"ru\"><head><meta charset=\"UTF-8\">");
  html += F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
  html += F("<title>Lab3 — Мониторинг</title>");
  html += FPSTR(STYLE);
  html += F("</head><body><div class=\"wrap\">");
  html += F("<nav><a href=\"/led\">Светодиод</a><a href=\"/data\">Мониторинг</a></nav>");
  html += F("<h1>Мониторинг данных</h1>");
  html += F("<p>Циклический буфер (30 значений), обновление каждую секунду</p>");
  html += F("<canvas id=\"chart\" width=\"640\" height=\"280\"></canvas>");
  html += F("<script>");
  html += F("const c=document.getElementById('chart');");
  html += F("const ctx=c.getContext('2d');");
  html += F("function draw(values){");
  html += F("ctx.clearRect(0,0,c.width,c.height);");
  html += F("if(!values.length)return;");
  html += F("const pad=24;const w=c.width-pad*2;const h=c.height-pad*2;");
  html += F("let min=Math.min(...values),max=Math.max(...values);");
  html += F("if(min===max){min-=1;max+=1;}");
  html += F("ctx.strokeStyle='#e2e8f0';");
  html += F("for(let i=0;i<=4;i++){");
  html += F("const y=pad+h*i/4;ctx.beginPath();ctx.moveTo(pad,y);ctx.lineTo(pad+w,y);ctx.stroke();}");
  html += F("ctx.strokeStyle='#2563eb';ctx.lineWidth=2;ctx.beginPath();");
  html += F("values.forEach((v,i)=>{");
  html += F("const x=pad+w*i/(values.length-1||1);");
  html += F("const y=pad+h*(1-(v-min)/(max-min));");
  html += F("if(i===0)ctx.moveTo(x,y);else ctx.lineTo(x,y);");
  html += F("});ctx.stroke();");
  html += F("ctx.fillStyle='#2563eb';");
  html += F("values.forEach((v,i)=>{");
  html += F("const x=pad+w*i/(values.length-1||1);");
  html += F("const y=pad+h*(1-(v-min)/(max-min));");
  html += F("ctx.beginPath();ctx.arc(x,y,3,0,Math.PI*2);ctx.fill();");
  html += F("});}");
  html += F("async function poll(){");
  html += F("const r=await fetch('/api/data');");
  html += F("const j=await r.json();");
  html += F("draw(j.values||[]);");
  html += F("}");
  html += F("poll();setInterval(poll,1000);");
  html += F("</script></div></body></html>");
  return html;
}

}  // namespace WebPages
