function BBMXM_setup()
  bbmxM_declare_func("hws_fx_add", {"string"})
end

function hws_fx_add(fx)
  bbmxM_send_cmd(30, {})
end