let SessionLoad = 1
if &cp | set nocp | endif
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd ~
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
if &shortmess =~ 'A'
  set shortmess=aoOA
else
  set shortmess=aoO
endif
badd +1 src/arduino/rampartII/firmware/rampart3-bytebeats/rampart3-bytebeats.ino
badd +0 src/arduino/rampartII/firmware/rampart3-bytebeats/bytebeats.h
badd +3 src/arduino/rampartII/firmware/rampart3-bytebeats/button.h
badd +54 src/arduino/rampartII/firmware/rampart3-bytebeats/LocalEncoderButton.h
badd +0 src/arduino/rampartII/firmware/rampart3-bytebeats/nyblybyte.h
argglobal
%argdel
set stal=2
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabrewind
edit src/arduino/rampartII/firmware/rampart3-bytebeats/rampart3-bytebeats.ino
argglobal
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
54
normal! zo
77
normal! zo
112
normal! zo
115
normal! zo
116
normal! zo
119
normal! zo
125
normal! zo
131
normal! zo
134
normal! zo
138
normal! zo
134
normal! zo
135
normal! zo
138
normal! zo
144
normal! zo
155
normal! zo
157
normal! zo
198
normal! zo
205
normal! zo
211
normal! zo
216
normal! zo
220
normal! zo
233
normal! zo
237
normal! zo
242
normal! zo
243
normal! zo
253
normal! zo
262
normal! zo
270
normal! zo
278
normal! zo
281
normal! zo
285
normal! zo
297
normal! zo
301
normal! zo
305
normal! zo
313
normal! zo
320
normal! zo
323
normal! zo
333
normal! zo
337
normal! zo
344
normal! zo
357
normal! zo
383
normal! zo
384
normal! zo
223
normal! zo
233
normal! zo
237
normal! zo
242
normal! zo
243
normal! zo
253
normal! zo
264
normal! zo
273
normal! zo
274
normal! zo
280
normal! zo
284
normal! zo
293
normal! zo
297
normal! zo
304
normal! zo
310
normal! zo
317
normal! zo
323
normal! zo
335
normal! zo
343
normal! zo
344
normal! zo
375
normal! zo
273
normal! zo
274
normal! zo
280
normal! zo
284
normal! zo
293
normal! zo
297
normal! zo
304
normal! zo
310
normal! zo
317
normal! zo
323
normal! zo
335
normal! zo
339
normal! zo
351
normal! zo
352
normal! zo
378
normal! zo
378
normal! zc
let s:l = 204 - ((44 * winheight(0) + 25) / 51)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 204
normal! 05|
lcd ~/src/arduino/rampartII/firmware/rampart3-bytebeats
tabnext
edit ~/src/arduino/rampartII/firmware/rampart3-bytebeats/nyblybyte.h
argglobal
balt ~/src/arduino/rampartII/firmware/rampart3-bytebeats/rampart3-bytebeats.ino
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
44
normal! zo
let s:l = 45 - ((44 * winheight(0) + 25) / 51)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 45
normal! 0
lcd ~/src/arduino/rampartII/firmware/rampart3-bytebeats
tabnext
edit ~/src/arduino/rampartII/firmware/rampart3-bytebeats/bytebeats.h
argglobal
balt ~/src/arduino/rampartII/firmware/rampart3-bytebeats/rampart3-bytebeats.ino
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
68
normal! zo
91
normal! zo
93
normal! zo
537
normal! zo
let s:l = 210 - ((5 * winheight(0) + 25) / 51)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 210
normal! 012|
lcd ~/src/arduino/rampartII/firmware/rampart3-bytebeats
tabnext 3
set stal=1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
