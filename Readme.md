# OGE

The intention of this repository is to create a platform with which to play with 3D graphics.
The goal is to be able to load shaders and model files from disk, and manipulate their parameters
through a variety of user interfaces (using [Dear ImGui](https://github.com/ocornut/imgui)) in order to help
me to learn more about 3D graphics and shader programming.

I also plan to implement saving and loading the current configuration to disk.

Features:
[ ] Shaders
    [x] Load shaders from disk
    [x] Link shader programs
    [x] Show loaded shaders and programs in interface
    [ ] Allow loading of shaders from interface
    [ ] Allow creation of programs from interface
[ ] Models
    [ ] Load models from disk
        [ ] .obj files
        [ ] other fie formats?
        [ ] custom file format?
[ ] Textures
    [ ] Load image files from disk
    [ ] Texture management in UI
        [ ] Texture previews?
[ ] Console
    [ ] Debug output
    [ ] Creation of variables (similar to quake engine cvars)
    [ ] Basic behaviours (e.g. binding a variable to a key)
    [ ] Console based manipulation of assets and objects
[ ] Objects
    [ ] Create objects from model + shader program
    [ ] Allow management of objects from UI
        [ ] Creation
        [ ] Deletion
        [ ] Management of shader uniforms
            [ ] Basic int, float uniforms
            [ ] Texture uniforms
            [ ] Bind uniforms to console variables
