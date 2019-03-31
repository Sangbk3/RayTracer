-- A simple scene with five spheres
jutext = gr.texture('jupiter.png')
eartext = gr.texture('earth.png')
mootext = gr.texture('moon.png')
spacetext = gr.texture('space.png')
cubebump = gr.texture('cubebump.png')

mat1 = gr.sangmaterial({0.2, 0.2, 0.3}, {1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, 25, 1, 10000)
gold = gr.material({0.8, 0.8, 0.2}, {0.8, 0.8, 0.4}, 5)
mat2 = gr.material({0.5, 0.9, 0.9}, {0.25, 0.35, 0.25}, 25)
matsang = gr.sangmaterial({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 25, 1.12, 100000)
matsang2 = gr.sangmaterial({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 25, 1.1, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.25, 0.35, 0.25}, 25)

mat4 = gr.material({1.0, 1.0, 1.0}, {0.2, 0.2, 0.2}, 25)
mat4:set_texture(eartext)
mat5 = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 25)
mat5:set_texture(mootext)
mat6 = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 25)
mat6:set_texture(jutext)
mat7 = gr.material({1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, 25)
mat7:set_texture(spacetext)

scene_root = gr.node('root')

-- plane = gr.mesh('wall1', 'plane.obj' )
-- scene_root:add_child(plane)
-- plane:set_material(gold)
-- plane:rotate('X', 90)
-- plane:scale(900, 900, 900)
-- plane:translate(0, 0, -1500)

s1 = gr.nh_sphere('s1', {0, 0, 700}, 40)
s1:scale(2, 2, 1)
scene_root:add_child(s1)
s1:set_material(matsang)

-- s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
-- scene_root:add_child(s2)
-- s2:set_material(mat4)

s3 = gr.nh_sphere('s3', {300, -700, -500}, 700)
scene_root:add_child(s3)
s3:set_material(mat6)

-- s4 = gr.nh_sphere('s4', {-100, 25, -300}, 50)
-- scene_root:add_child(s4)
-- s4:set_material(mat2)

-- s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
-- scene_root:add_child(s5)
-- s5:set_material(mat3)


-- s7 = gr.nh_sphere('s7', {-100, 0, 200}, 150)
-- scene_root:add_child(s7)
-- s7:set_material(mat5)


s11 = gr.nh_sphere('s11', {-200, 200, -100}, 150)
scene_root:add_child(s11)
s11:set_material(mat5)


-- s8 = gr.nh_sphere('s8', {10, -100, -250}, 25)
-- scene_root:add_child(s8)
-- s8:set_material(mat3)


-- s9 = gr.nh_sphere('s9', {-50, 300, -250}, 25)
-- scene_root:add_child(s9)
-- s9:set_material(mat3)

s6 = gr.nh_sphere('s6', {0, 0, 800}, 2200)
scene_root:add_child(s6)
s6:scale(1, 1, 1)
s6:set_material(mat7)

white_light = gr.arealight({-400.0, 50.0, 500.0}, {0.9, 0.9, 0.9}, {1, 0.0, 0.0}, 30)
-- magenta_light = gr.arealight({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0.0, 0.0}, 30)

gr.render(scene_root, 'simple.png', 1024, 1024,
	  {0, 0, 1000}, {0, 0, -800}, {0, 1, 0}, 50,
	  {0.1, 0.1, 0.1}, {white_light, magenta_light})
