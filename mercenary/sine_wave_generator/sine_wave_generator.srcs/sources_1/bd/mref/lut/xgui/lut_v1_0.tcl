# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "ADDRESS_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "DATA_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "FILENAME" -parent ${Page_0}


}

proc update_PARAM_VALUE.ADDRESS_WIDTH { PARAM_VALUE.ADDRESS_WIDTH } {
	# Procedure called to update ADDRESS_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.ADDRESS_WIDTH { PARAM_VALUE.ADDRESS_WIDTH } {
	# Procedure called to validate ADDRESS_WIDTH
	return true
}

proc update_PARAM_VALUE.DATA_WIDTH { PARAM_VALUE.DATA_WIDTH } {
	# Procedure called to update DATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.DATA_WIDTH { PARAM_VALUE.DATA_WIDTH } {
	# Procedure called to validate DATA_WIDTH
	return true
}

proc update_PARAM_VALUE.FILENAME { PARAM_VALUE.FILENAME } {
	# Procedure called to update FILENAME when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.FILENAME { PARAM_VALUE.FILENAME } {
	# Procedure called to validate FILENAME
	return true
}


proc update_MODELPARAM_VALUE.ADDRESS_WIDTH { MODELPARAM_VALUE.ADDRESS_WIDTH PARAM_VALUE.ADDRESS_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.ADDRESS_WIDTH}] ${MODELPARAM_VALUE.ADDRESS_WIDTH}
}

proc update_MODELPARAM_VALUE.DATA_WIDTH { MODELPARAM_VALUE.DATA_WIDTH PARAM_VALUE.DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.DATA_WIDTH}] ${MODELPARAM_VALUE.DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.FILENAME { MODELPARAM_VALUE.FILENAME PARAM_VALUE.FILENAME } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.FILENAME}] ${MODELPARAM_VALUE.FILENAME}
}

