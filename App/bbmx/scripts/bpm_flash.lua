require("bbmx")

local r
local g
local b
local w

function BBMX_setup()
  bbmx_port("COM4")

  bbmx_using("LM108")

  bbmx_fixture("fx1")

  bbmx_snd("test.ogg", 128)
end

function BBMX_start()
  bbmx_fx_reset("fx1")

  r = 255
  g = 255
  b = 255
  w = 255

  bbmx_fixture("fx1")

  bbmx_fx_tilt("fx1", 135, 1)
  bbmx_fx_brgt("fx1", 20)
end

function BBMX_beat()
  r = 255
  g = 255
  b = 255
  w = 255
end

local waited = 0

function BBMX_loop(delta)
  r = lerp(r, 0, 0.005 * delta)
  g = lerp(g, 0, 0.005 * delta)
  b = lerp(b, 0, 0.005 * delta)
  w = lerp(w, 0, 0.005 * delta)
  
  bbmx_fx_rgbw("fx1", math.floor(math.abs(r)), math.floor(math.abs(g)), math.floor(math.abs(b)), math.floor(math.abs(w)))
  
  waited = waited + delta
  if waited >= 1000 then
    waited = 0
    bbmx_fx_tilt("fx1", math.random(50, 230), 1)
    bbmx_fx_pan("fx1", math.random(0, 540), 1)
  end
end