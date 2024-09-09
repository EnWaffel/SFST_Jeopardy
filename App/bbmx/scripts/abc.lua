function BBMX_setup()
  bbmx_port("COM4")

  bbmx_using("LM108")

  bbmx_fixture("fx1")
end

function BBMX_start()
  bbmx_fx_tilt("fx1", 135, 1)
  bbmx_fx_brgt("fx1", 255)
  bbmx_fx_rgbw("fx1", 255, 255, 255, 255)
end
