function BBMX_setup()
  bbmx_port("COM4")

  bbmx_using("LM108")

  bbmx_fixture("fx1")
end

function BBMX_start()
  bbmx_fx_brgt("fx1", 255)
end

local lvl = 150.0

function BBMX_loop(delta)
  lvl = lvl - 0.1 * delta
  if lvl <= 0 then bbmx_fx_w("fx1", 0) bbmx_exit() return end
  bbmx_fx_w("fx1", math.floor(lvl))
end