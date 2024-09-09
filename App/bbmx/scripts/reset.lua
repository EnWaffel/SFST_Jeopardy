function BBMX_setup()
  bbmx_port("COM4")

  bbmx_using("LM108")

  bbmx_fixture("fx1")
end

function BBMX_start()
  bbmx_fx_reset("fx1")
end