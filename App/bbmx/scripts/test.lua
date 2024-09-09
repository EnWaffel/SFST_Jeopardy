function BBMX_setup()
  bbmx_port("COM4")

  bbmx_using("LM108")

  bbmx_fixture("fx 1")
  bbmx_fixture("fx 2")

  bbmx_group("group 1", {"fx 1", "fx 2"})
end

function BBMX_start()
end

function BBMX_loop(delta)
  if time >= 1000 and time < 1001 then
    bbmx_fx_brgt("fx 1", 30)
    bbmx_fx_rgbw("fx 1", 255, 255, 255, 255)
    bbmx_fx_tilt("fx 1", 135, 1)
    bbmx_fx_pan("fx 1", 270, 1)
  end

  if time >= 5000 and time < 5001 then
    bbmx_fx_rgbw("fx 1", 0, 0, 255, 255)
    bbmx_fx_tilt("fx 1", 270, 1)
    bbmx_fx_pan("fx 1", 170, 1)
  end
  if time >= 7000 and time < 7001 then
    bbmx_fx_rgbw("fx 1", 0, 0, 0, 255)
    bbmx_fx_tilt("fx 1", 0, 1)
    bbmx_fx_pan("fx 1", 0, 1)
    bbmx_exit()
  end
end
