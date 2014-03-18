# TODO: Resolve duplicate data

import os, struct


class Vec3:
    """Stores vertex and normal data"""
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z


class Vec2:
    """Stores UV data"""
    def __init__(self, u, v):
        self.u = u
        self.v = v


class Face:
    """Basically a 3x3 matrix"""
    def __init__(self, vert, tex, norm):
        self.vert = vert
        self.tex = tex
        self.norm = norm


class Model:
    def __init__(self):
        self.vertices = list()
        self.texcoords = list()
        self.normals = list()
        self.faces = list()
        # Expected texture, if found
        self.material = ""
        # Used for splitting objects into separate meshes
        self.offset = Face(0, 0, 0)
        return

    def LoadOBJ(self, filename, split_mesh=False):
        try:
            f = open(filename, 'r')
        except:
            print "Object file not found"
            return
        # For split mesh mode
        mesh_count = 1
        obj_name = filename.rsplit(".", 1)[0]
        if split_mesh is True:
            try:
                os.remove(obj_name + ".csv")
            except:
                pass
        # Append "o" to ensure all objects are written out in mesh split mode
        for line in f.readlines()+["o"]:
            data = line.split()
            head = data[0] if len(data) > 0 else ""
            if (head == "o" or head == "g") and split_mesh is True:
                if len(self.faces) > 0:
                    save_name = "{}_{}.mesh".format(obj_name, mesh_count)
                    with open(obj_name + ".csv", "a") as csv:
                        csv.write("{},{}\n".format(save_name.split("/")[-1],
                                                   self.material))
                    self.SaveMesh(save_name)
                    mesh_count += 1
            elif head == "usemtl":
                self.material = data[1]
            elif head == "v":
                vert = Vec3(*map(float, data[1:4]))
                self.vertices.append(vert)
            elif head == "vt":
                uv = Vec2(*map(float, data[1:3]))
                self.texcoords.append(uv)
            elif head == "vn":
                norm = Vec3(*map(float, data[1:4]))
                self.normals.append(norm)
            elif head == "f":
                vert = list()
                tex = list()
                norm = list()

                if len(data) < 4:
                    raise ValueError("Missing face value")

                for i in data[1:4]:
                    i = i.split('/')
                    l = len(i)

                    if l < 1 or i[0] == "":
                        raise ValueError("Face missing vertices")
                    if l < 2 or i[1] == "":
                        self.texcoords = list()
                    if l < 3 or i[2] == "":
                        self.normals = list()

                    vert.append(int(i[0]))
                    if len(self.texcoords):
                        tex.append(int(i[1]))
                    if len(self.normals):
                        norm.append(int(i[2]))
                self.faces.append(Face(vert, tex, norm))
                # Check if it's a rude quad
                if len(data) > 4:
                    # Build another tri
                    i = data[4].split('/')
                    l = len(i)

                    if l < 1 or i[0] == "":
                        raise ValueError("Face missing vertices")
                    if l < 2 or i[1] == "":
                        self.texcoords = list()
                    if l < 3 or i[2] == "":
                        self.normals = list()

                    # Getting copies is a pain
                    old = self.faces[-1]
                    new_tri = Face(old.vert[:], old.tex[:], old.norm[:])

                    # Replace vert[1] with new vert
                    # Swap to fix winding
                    new_tri.vert[0], new_tri.vert[1] = \
                        int(i[0]), new_tri.vert[0]
                    if len(self.texcoords):
                        new_tri.tex[0], new_tri.tex[1] = \
                            int(i[1]), new_tri.tex[0]
                    if len(self.normals):
                        new_tri.norm[0], new_tri.norm[1] = \
                            int(i[2]), new_tri.norm[0]
                    self.faces.append(new_tri)

        return

    def LoadCollada(self, filename):
        return

    def SaveMesh(self, filename):
        try:
            f = open(filename, 'wb+')
        except:
            print "Could not open save file"
            return
        mod_ui = "=I"
        mod_f = "=f"
        mod_2f = "=ff"
        mod_3f = "=fff"

        # Adjust for offset in file
        for face in self.faces:
            for i in xrange(3):
                face.vert[i] = face.vert[i] - self.offset.vert
                if len(self.texcoords):
                    face.tex[i] = face.tex[i] - self.offset.tex
                if len(self.normals):
                    face.norm[i] = face.norm[i] - self.offset.norm

        # Output vert, uv, normal, and face counts
        f.write(struct.pack(mod_ui, len(self.vertices)))
        f.write(struct.pack(mod_ui, len(self.texcoords)))
        f.write(struct.pack(mod_ui, len(self.normals)))
        f.write(struct.pack(mod_ui, len(self.faces)))

        # Output vertex data
        for vert in self.vertices:
            f.write(struct.pack(mod_3f, vert.x, vert.y, vert.z))

        # Output UV data
        for texel in self.texcoords:
            f.write(struct.pack(mod_2f, texel.u, texel.v))

        # Output normal data
        for normal in self.normals:
            f.write(struct.pack(mod_3f, normal.x, normal.y, normal.z))

        # Output face data
        for face in self.faces:
            for i in xrange(3):
                f.write(struct.pack(mod_ui, face.vert[i]))
                if len(self.texcoords):
                    f.write(struct.pack(mod_ui, face.tex[i]))
                if len(self.normals):
                    f.write(struct.pack(mod_ui, face.norm[i]))

        f.close()

        # Record new offset
        self.offset.vert = self.offset.vert + len(self.vertices)
        self.offset.tex = self.offset.tex + len(self.texcoords)
        self.offset.norm = self.offset.norm + len(self.normals)
        # Reset our mesh data
        del self.vertices[:]
        del self.texcoords[:]
        del self.normals[:]
        del self.faces[:]
        self.material = ""
        return

if __name__ == "__main__":
    m = Model()
    m.LoadOBJ("../notes/sponza/sponza.obj", False)
    # print "Verts:"
    # for vert in m.vertices:
    #     print [vert.x, vert.y, vert.z]
    # print "UVs:"
    # for tex in m.texcoords:
    #     print [tex.u, tex.v]
    # print "Normals:"
    # for norm in m.normals:
    #     print [norm.x, norm.y, norm.z]
    # print "Faces:"
    # for face in m.faces:
    #     print [face.vert, face.tex, face.norm]
    print ("Size:\n\tVerts:{0}\n\tUVs:{1}\n\t"
           "Normals:{2}\n\tPolys:{3}\n").format(len(m.vertices),
                                                len(m.texcoords),
                                                len(m.normals),
                                                len(m.faces))
    m.SaveMesh("../notes/sponza/sponza.mesh")
