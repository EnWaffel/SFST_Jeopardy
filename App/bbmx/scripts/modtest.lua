require("bbmx")

function BBMX_setup()
  bbmx_port("COM4")
  bbmx_using("LM108")
  bbmx_fixture("fx1")
  bbmx_module("hardwaresync")
end

function BBMX_start()
  bbmx_fx_brgt("fx1", 50)
  bbmx_mod_func("hws_fx_add", "fx1")
end