

function BBMX_setup()
  bbmx_port("COM4")

  bbmx_using("LM108")

  bbmx_fixture("fx1")
end

function BBMX_start()
  bbmx_fx_brgt("fx1", 255)
  bbmx_fx_rgb("fx1", math.floor(255 * 0.5), math.floor(255 * 0.5), 0)
end