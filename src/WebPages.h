#pragma once

#include <Arduino.h>

namespace WebPages {

constexpr char STYLE[] PROGMEM = R"raw(
<style>
  * { box-sizing: border-box; }
  body { font-family: Arial, sans-serif; margin: 0; background: #f0f4f8; color: #1f2937; }
  .wrap { max-width: 720px; margin: 40px auto; padding: 24px; background: #fff;
    border-radius: 12px; box-shadow: 0 8px 24px rgba(0,0,0,.08); }
  h1, h2 { margin-top: 0; }
  h2 { font-size: 1.1rem; margin: 24px 0 8px; }
  nav { margin-bottom: 20px; }
  nav a { margin-right: 12px; color: #2563eb; text-decoration: none; font-weight: 600; }
  label { display: block; margin: 12px 0 6px; font-weight: 600; }
  input[type=text], input[type=password] {
    width: 100%; padding: 10px 12px; border: 1px solid #cbd5e1; border-radius: 8px; }
  button { margin-top: 12px; margin-right: 8px; padding: 10px 16px; border: none;
    border-radius: 8px; font-size: 1rem; cursor: pointer; background: #2563eb; color: #fff; }
  button.danger { background: #dc2626; }
  .status { margin-top: 16px; padding: 12px; border-radius: 8px; background: #eff6ff; font-weight: 600; }
  .error { color: #dc2626; margin-bottom: 12px; }
  .chart-box {
    width: 100%; height: 240px; border: 1px solid #e2e8f0;
    border-radius: 8px; background: #fff; display: block;
  }
</style>
)raw";

inline String loginPage(bool showError) {
  String html = F("<!DOCTYPE html><html lang=\"ru\"><head><meta charset=\"UTF-8\">");
  html += F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
  html += F("<title>Lab3 — Вход</title>");
  html += FPSTR(STYLE);
  html += F("</head><body><div class=\"wrap\"><h1>Аутентификация</h1>");
  if (showError) {
    html += F("<p class=\"error\">Неверный логин или пароль</p>");
  }
  html += F("<form method=\"POST\" action=\"/login\">");
  html += F("<label for=\"username\">Логин</label>");
  html += F("<input id=\"username\" name=\"username\" type=\"text\" required>");
  html += F("<label for=\"password\">Пароль</label>");
  html += F("<input id=\"password\" name=\"password\" type=\"password\" required>");
  html += F("<button type=\"submit\">Войти</button></form></div></body></html>");
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
  html += F("const opts={credentials:'include'};");
  html += F("async function refresh(){const r=await fetch('/api/led/state',opts);const j=await r.json();");
  html += F("document.getElementById('status').textContent='Состояние: '+(j.on?'Включен':'Выключен');}");
  html += F("document.getElementById('btnOn').onclick=async()=>{await fetch('/api/led/on',{method:'POST',...opts});refresh();};");
  html += F("document.getElementById('btnOff').onclick=async()=>{await fetch('/api/led/off',{method:'POST',...opts});refresh();};");
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
  html += F("<h1>Мониторинг AHT20</h1>");
  html += F("<p>30 последних значений, обновление каждую секунду (без перезагрузки)</p>");
  html += F("<p id=\"dataInfo\">Загрузка...</p><p class=\"error\" id=\"dataErr\"></p>");
  html += F("<h2>Температура, °C</h2>");
  html += F("<svg id=\"chartTemp\" class=\"chart-box\" viewBox=\"0 0 600 230\" preserveAspectRatio=\"none\"></svg>");
  html += F("<h2>Влажность, %</h2>");
  html += F("<svg id=\"chartHum\" class=\"chart-box\" viewBox=\"0 0 600 230\" preserveAspectRatio=\"none\"></svg>");
  html += F("<script>");
  html += F("function toNums(a){var o=[];for(var i=0;i<a.length;i++)o.push(Number(a[i]));return o;}");
  html += F("function fmtTime(sec,t0){");
  html += F("if(sec<1000000000){var d=sec-(t0||0),m=Math.floor(d/60),s=d%60;");
  html += F("return ('0'+m).slice(-2)+':'+('0'+s).slice(-2);}");
  html += F("var d=new Date(sec*1000);");
  html += F("var h=('0'+d.getHours()).slice(-2),m=('0'+d.getMinutes()).slice(-2);");
  html += F("var s=('0'+d.getSeconds()).slice(-2);return h+':'+m+':'+s;}");
  html += F("function timeAxis(times,w,pad,den,baseY){");
  html += F("var lbl='',n=Math.min(6,times.length),k,idx,x,t0=times[0]||0;");
  html += F("for(k=0;k<n;k++){idx=(n===1)?0:Math.round(k*(times.length-1)/(n-1));");
  html += F("x=pad+(w-2*pad)*idx/den;");
  html += F("lbl+='<text x=\"'+x+'\" y=\"'+baseY+'\" font-size=\"10\" fill=\"#64748b\" text-anchor=\"middle\">'+fmtTime(times[idx],t0)+'</text>';}");
  html += F("return lbl;}");
  html += F("function drawSvg(id,values,times,color){");
  html += F("var s=document.getElementById(id);if(!s)return;");
  html += F("if(!values||values.length<1){s.innerHTML='';return;}");
  html += F("var w=600,h=175,pad=30,baseY=215,i,x,y,den=values.length>1?values.length-1:1;");
  html += F("if(!times||times.length<values.length){times=[];for(i=0;i<values.length;i++)times.push(0);}");
  html += F("var mn=values[0],mx=values[0];");
  html += F("for(i=1;i<values.length;i++){if(values[i]<mn)mn=values[i];if(values[i]>mx)mx=values[i];}");
  html += F("if(mn===mx){mn-=1;mx+=1;}");
  html += F("var grid='';for(i=0;i<=4;i++){y=pad+(h-2*pad)*i/4;");
  html += F("grid+='<line x1=\"'+pad+'\" y1=\"'+y+'\" x2=\"'+(w-pad)+'\" y2=\"'+y+'\" stroke=\"#e2e8f0\"/>';}");
  html += F("var pts='';for(i=0;i<values.length;i++){");
  html += F("x=pad+(w-2*pad)*i/den;y=pad+(h-2*pad)*(1-(values[i]-mn)/(mx-mn));");
  html += F("pts+=x+','+y+' ';}");
  html += F("s.innerHTML=grid+'<text x=\"4\" y=\"'+pad+'\" font-size=\"11\" fill=\"#64748b\">'+mx.toFixed(1)+'</text>'");
  html += F("+'<text x=\"4\" y=\"'+(pad+h-10)+'\" font-size=\"11\" fill=\"#64748b\">'+mn.toFixed(1)+'</text>'");
  html += F("+'<polyline points=\"'+pts+'\" fill=\"none\" stroke=\"'+color+'\" stroke-width=\"2\"/>'");
  html += F("+timeAxis(times,w,pad,den,baseY);}");
  html += F("function poll(){");
  html += F("fetch('/api/data',{credentials:'same-origin'}).then(function(r){");
  html += F("if(!r.ok)throw new Error(r.status);return r.json();}).then(function(j){");
  html += F("document.getElementById('dataErr').textContent='';");
  html += F("var t=toNums(j.temperature||[]),h=toNums(j.humidity||[]),ts=toNums(j.timestamps||[]);");
  html += F("document.getElementById('dataInfo').textContent=");
  html += F("'Сейчас: '+j.currentTemp+' C, '+j.currentHum+' % | точек: '+t.length;");
  html += F("drawSvg('chartTemp',t,ts,'#dc2626');drawSvg('chartHum',h,ts,'#2563eb');");
  html += F("}).catch(function(e){");
  html += F("document.getElementById('dataErr').textContent=");
  html += F("'Ошибка: '+e+'. Откройте http://IP/data после входа.';});}");
  html += F("poll();setInterval(poll,1000);</script></div></body></html>");
  return html;
}

}  // namespace WebPages
