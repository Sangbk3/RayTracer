-- A simple scene with five spheres

mat1 = gr.sangmaterial({0.2, 0.2, 0.3}, {1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, 25, 1, 10000)
gold = gr.material({0.8, 0.8, 0.2}, {0.8, 0.8, 0.4}, 5)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.25, 0.35, 0.25}, 25)
matsang = gr.sangmaterial({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 25, 1.5, 100000)
matsang2 = gr.sangmaterial({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 25, 1.1, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.25, 0.35, 0.25}, 25)

scene_root = gr.node('root')

-- plane = gr.mesh('wall1', 'plane.obj' )
-- scene_root:add_child(plane)
-- plane:set_material(gold)
-- plane:rotate('X', 90)
-- plane:scale(900, 900, 900)
-- plane:translate(0, 0, -1500)

s1 = gr.nh_sphere('s1', {0, 0, -400}, 100)
scene_root:add_child(s1)
s1:set_material(mat2)

s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
scene_root:add_child(s2)
s2:set_material(mat2)

s3 = gr.nh_sphere('s3', {0, -1200, -500}, 1000)
scene_root:add_child(s3)
s3:set_material(mat1)

s4 = gr.nh_sphere('s4', {-100, 25, -300}, 50)
scene_root:add_child(s4)
s4:set_material(mat2)

s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
scene_root:add_child(s5)
s5:set_material(mat3)


s7 = gr.nh_sphere('s7', {-10, 0, 500}, 50)
scene_root:add_child(s7)
s7:set_material(matsang)


s8 = gr.nh_sphere('s8', {10, -100, -250}, 25)
scene_root:add_child(s8)
s8:set_material(mat3)


s9 = gr.nh_sphere('s9', {-50, 300, -250}, 25)
scene_root:add_child(s9)
s9:set_material(mat3)

s6 = gr.nh_sphere('s6', {0, 0, 800}, 5)
scene_root:add_child(s6)
s6:scale(1, 1, 1)
s6:set_material(matsang)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0.0, 0.0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0.0, 0.0})

gr.render(scene_root, 'simple.png', 1024, 1024,
	  {0, 0, 800}, {0, 0, -800}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})
