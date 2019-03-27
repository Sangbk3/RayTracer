
ground = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 5)
gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)

-- ##############################################
-- the scene
-- ##############################################

scene = gr.node('scene')
scene:rotate('X', 23)

-- the floor

plane = gr.mesh('plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(gold)
plane:scale(30, 30, 30)

-- the wall
plane = gr.mesh('wall1', 'plane.obj' )
scene:add_child(plane)
plane:set_material(ground)
plane:rotate('X', -90)
plane:scale(30, 30, 30)
plane:translate(0, 0, -20)

plane = gr.mesh('wall2', 'plane.obj' )
scene:add_child(plane)
plane:set_material(ground)
plane:rotate('Z', -90)
plane:scale(30, 30, 30)
plane:translate(5, 0, 0)

plane = gr.mesh('wall3', 'plane.obj' )
scene:add_child(plane)
plane:set_material(ground)
plane:rotate('Z', 90)
plane:scale(30, 30, 30)
plane:translate(-5, 0, 0)

plane = gr.mesh('wall4', 'plane.obj' )
scene:add_child(plane)
plane:set_material(ground)
plane:rotate('X', 90)
plane:scale(30, 30, 30)
plane:translate(0, 0, 30)

mickey_instance = gr.mesh('mickey', 'mickey.obj')
scene:add_child(mickey_instance)
mickey_instance:rotate('X', 90)
mickey_instance:rotate('Z', 140)
mickey_instance:scale(10, 10, 10)
mickey_instance:translate(1, -0.5, 0)
mickey_instance:set_material(blue)


gr.render(scene,
	  'sample-b.png', 1024, 1024,
	  {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 70,
      {0.4, 0.4, 0.4}, {gr.light({0, 200, 300}, {1.0, 1.0, 1.0}, {1, 0, 0})})
