
code1 = """
	sn_err,push_char,push_int,push_long,push_vchar,push_vint,push_vlong,
	push_gchar,push_gint,push_glong,push_achar,push_aint,push_along,push_string,
	push_lvchar,push_lvint,push_lvlong,push_lgchar,push_lgint,push_lglong,
	push_lachar,push_laint,push_lalong,push_along,push_lalong,push_llong,
	push_text,push_graph,push_sub0,cal_inc,cal_dec,cal_INC,cal_DEC,
	cal_add,cal_sub,cal_and,cal_or,push_not,cal_xor,cal_land,cal_lor,cal_lnot,
	cal_mul,cal_div,cal_mod,cal_lshift,cal_rshift,cal_equ,cal_neq,cal_le,cal_ge,cal_great,cal_less,
	c_let,c_ptr,c_cptr,pop_val,c_jmpe,c_jmpn,c_jmp,set_sp,c_call,add_bp,sub_bp,
	good_exit,c_preset,push_gbuf,c_secret,c_loadall,
	cal_qadd,cal_qsub,cal_qmul,cal_qdiv,cal_qmod,cal_qlshift,cal_qrshift,
	cal_qequ,cal_qneq,cal_qgreat,cal_qless,cal_qge,cal_qle,
	c_iptr,c_lptr,
	c_icf,c_fci,cal_addff,cal_addf,cal_add0f,cal_subff,cal_subf,cal_sub0f,
	cal_mulff,cal_mulf,cal_mul0f,cal_divff,cal_divf,cal_div0f,push_sub0f,
	cal_lessf,cal_greatf,cal_equf,cal_neqf,cal_lef,cal_gef,c_f0,
	c_ciptr,c_clptr,c_lcc,c_lci,c_letx,push_ax,cal_idx,c_pass,c_void,
	c_debug,c_funcid
"""

code1 = code1.replace('\n', '').replace('\t', '').split(',')
print(code1)

for i in range(0, len(code1)):
    print('0x%02x => %s' % (i, code1[i]))