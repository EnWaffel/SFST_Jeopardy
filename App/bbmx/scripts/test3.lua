local r
local g
local b
local w
local beatRGB = false
local beatW = true

function t1()
  r = 255
  g = 255
  b = 255
end

function t2()
  beatW = false
end

function t3()
  beatRGB = true
end

function t4()
  beatW = true
end

function t5()
  beatRGB = false
  beatW = false
end

function t6()
  g = 255
  b = 255
end

function t7()
  r = 255
  g = 255
  b = 255
  w = 255
end
function t8()
  r = 255
  g = 255
  b = 255
  w = 255
end
function t9()
  r = 255
  g = 255
  b = 255
  w = 255
end
function t10()
  r = 255
  g = 255
  b = 255
  w = 255
end

function BBMX_setup()
  bbmx_port("COM4")

  bbmx_using("LM108")

  bbmx_fixture("fx1")

  bbmx_snd("test.ogg", 150)

  r = 255
  g = 255
  b = 255
  w = 255

  bbmx_timed("t1", 6800)
  bbmx_timed("t2", 11167)
  bbmx_timed("t3", 12800)
  bbmx_timed("t4", 19229)
  bbmx_timed("t5", 25673)
  bbmx_timed("t6", 28900)
  bbmx_timed("t7", 30424)
  bbmx_timed("t8", 30825)
  bbmx_timed("t9", 31232)
  bbmx_timed("t10", 31633)
end

function BBMX_start()
  bbmx_fx_brgt("fx1", 50)
  bbmx_fx_rgbw("fx1", 0, 0, 0, 0)
  bbmx_fx_tilt("fx1", 135, 1)
end

function BBMX_beat()
  if beatW then
    w = 255
  end
  if beatRGB then
    r = 255
    g = 255
    b = 255
  end
end

function BBMX_loop(delta)
  r = lerp(r, 0, 0.005 * delta)
  g = lerp(g, 0, 0.005 * delta)
  b = lerp(b, 0, 0.005 * delta)
  w = lerp(w, 0, 0.005 * delta)
  
  bbmx_fx_rgbw("fx1", math.floor(math.abs(r)), math.floor(math.abs(g)), math.floor(math.abs(b)), math.floor(math.abs(w)))
end
