; This implements a small demonstration of an API created in GT.M that
; can be called from a C main() program.
; No claim of copyright is made with respect to this code
;
; This program is only a demonstration.  Please ensure that you have a
; correctly configured GT.M installation.
;
; Please do not use this code as-is in a production environment.
;
%gtmaccess	; entry points to access GT.M
	quit
	;
get(var,value,error)
	set value=@var
	quit:$quit 0 quit
	;
init(error)
	set $ztrap="new tmp set error=$ecode set tmp=$piece($ecode,"","",2) quit:$quit $extract(tmp,2,$length(tmp)) quit"
	quit:$quit 0 quit
	;
kill(var,error)
	kill @var
	quit:$quit 0 quit
	;
lock(var,error)
	lock @var
	quit:$quit 0 quit
	;
order(var,value,error)
	set value=$order(@var)
	quit:$quit 0 quit
	;
query(var,value,error)
	set value=$query(@var)
	quit:$quit 0 quit
	;
set(var,value,error)
	set @var=value
	quit:$quit 0 quit
	;
xecute(var,error)
	xecute var
	quit:$quit 0 quit
	;
xecuteintrans(var,error)
	TSTART ():SERIAL
	;write "start trans"
	xecute var
	TCOMMIT
	;write "end trans"
	quit:$quit 0 quit
	;
toJSON(var,value,error)
	n LOM
	M LOM=@var
	set value=$$arrayToJSON("LOM")
	quit:$quit 0 quit
	;
arrayToJSON(name)
	n subscripts
	i '$d(@name) QUIT "[]"
	QUIT $$walkArray("",name)
	;
walkArray(json,name,subscripts)
	 ;
	 n allNumeric,arrComma,brace,comma,count,cr,dd,i,no,numsub,dblquot,quot
	 n ref,sub,subNo,subscripts1,type,valquot,value,xref,zobj
	 ;
	 s cr=$c(13,10),comma=","
	 s (dblquot,valquot)=""""
	 s dd=$d(@name)
	 i dd=1!(dd=11) d  i dd=1 QUIT json
	 . s value=@name
	 . i value'[">" q
	 . s json=$$walkArray(json,value,.subscripts)
	 s ref=name_"("
	 s no=$o(subscripts(""),-1)
	 i no>0 f i=1:1:no d
	 . s quot=""""
	 . i subscripts(i)?."-"1N.N s quot=""
	 . s ref=ref_quot_subscripts(i)_quot_","
	 s ref=ref_"sub)"
	 s sub="",numsub=0,subNo=0,count=0
	 s allNumeric=1
	 f  s sub=$o(@ref) q:sub=""  d  q:'allNumeric
	 . i sub'?1N.N s allNumeric=0
	 . s count=count+1
	 . i sub'=count s allNumeric=0
	 ;i allNumeric,count=1 s allNumeric=0
	 i allNumeric d
	 . s json=json_"["
	 e  d
	 . s json=json_"{"
	 s sub=""
	 f  s sub=$o(@ref) q:sub=""  d
	 . s subscripts(no+1)=sub
	 . s subNo=subNo+1
	 . s dd=$d(@ref)
	 . i dd=1 d
	 . . s value=@ref
	 . . i 'allNumeric d
	 . . . s json=json_""""_sub_""":"
	 . . s type="literal"
	 . . i $$numeric(value) s type="numeric"
	 . . ;i value?1N.N s type="numeric"
	 . . ;i value?1"-"1N.N s type="numeric"
	 . . ;i value?1N.N1"."1N.N s type="numeric"
	 . . ;i value?1"-"1N.N1"."1N.N s type="numeric"
	 . . i value="true"!(value="false") s type="boolean"
	 . . i $e(value,1)="{",$e(value,$l(value))="}" s type="variable"
	 . . i $e(value,1,4)="<?= ",$e(value,$l(value)-2,$l(value))=" ?>" d
	 . . . s type="variable"
	 . . . s value=$e(value,5,$l(value)-3)
	 . . i type="literal" s value=valquot_value_valquot
	 . . d
	 . . . s json=json_value_","
	 . k subscripts1
	 . m subscripts1=subscripts
	 . i dd>9 d
	 . . i sub?1N.N,allNumeric d
	 . . . i subNo=1 d
	 . . . . s numsub=1
	 . . . . s json=$e(json,1,$l(json)-1)
	 . . . . s json=json_"["
	 . . e  d
	 . . . s json=json_""""_sub_""":"
	 . . s json=$$walkArray(json,name,.subscripts1)
	 . . d
	 . . . s json=json_","
	 ;
	 s json=$e(json,1,$l(json)-1)
	 i allNumeric d
	 . s json=json_"]"
	 e  d
	 . s json=json_"}"
	 QUIT json ; exit!
	 ;
numeric(value)
	 i $e(value,1,9)="function(" QUIT 1
	 i value?1"0."1N.N QUIT 1
	 i $e(value,1)=0,$l(value)>1 QUIT 0
	 i $e(value,1,2)="-0",$l(value)>2,$e(value,1,3)'="-0." QUIT 0
	 i value?1N.N QUIT 1
	 i value?1"-"1N.N QUIT 1
	 i value?1N.N1"."1N.N QUIT 1
	 i value?1"-"1N.N1"."1N.N QUIT 1
	 i value?1"."1N.N QUIT 1
	 i value?1"-."1N.N QUIT 1
	 QUIT 0
	 ;
