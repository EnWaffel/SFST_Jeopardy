function t1()
  bbmx_fx_rgb("fx1", 255, 0, 0)
end

function t2()
  bbmx_fx_rgb("fx1", 0, 255, 0)
end

function t3()
  bbmx_fx_rgb("fx1", 0, 0, 255)
end

function t4()
  bbmx_reset_timer()
end

function BBMX_setup()
  bbmx_port("COM4")

  bbmx_using("LM108")

  bbmx_fixture("fx1")

  bbmx_timed("t1", 0)
  bbmx_timed("t2", 16)
  bbmx_timed("t3", 16 * 2)
  bbmx_timed("t4", 16 * 3)
end

function BBMX_start()
  bbmx_fx_brgt("fx1", 50)
  bbmx_fx_rgbw("fx1", 0, 0, 0, 0)
  bbmx_fx_tilt("fx1", 135, 1)
end
