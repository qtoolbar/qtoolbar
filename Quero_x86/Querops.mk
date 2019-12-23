
Querops.dll: dlldata.obj Quero_p.obj Quero_i.obj
	link /dll /out:Querops.dll /def:Querops.def /entry:DllMain dlldata.obj Quero_p.obj Quero_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del Querops.dll
	@del Querops.lib
	@del Querops.exp
	@del dlldata.obj
	@del Quero_p.obj
	@del Quero_i.obj
