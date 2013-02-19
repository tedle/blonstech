# TODO: Resolve duplicate data

import struct

# Stores vertex and normal data
class Vec3:
    def __init__(self, x, y, z):
        self.x = x;
        self.y = y;
        self.z = z;

# Stores UV data
class Vec2:
    def __init__(self, u, v):
        self.u = u;
        self.v = v;

# This is basically a 3x3 matrix
class Face:
    def __init__(self, vert, tex, norm):
        self.vert = vert
        self.tex  = tex
        self.norm = norm

class Model:
    def __init__(self):
        self.vertices  = list();
        self.texcoords = list();
        self.normals   = list();
        self.faces     = list();
        return

    def LoadOBJ(self, filename):
        try:
            f = open(filename, 'r')
        except:
            print "Object file not found"
            return
        for line in f.readlines():
            data = line.split()
            head = data[0] if len(data) > 0 else ""
            if head == "v":
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
                tex  = list()
                norm = list()

                if len(data) < 4:
                    raise ValueError("Missing face value")

                for i in data[1:]:
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
        mod_f  = "=f"
        mod_2f = "=ff"
        mod_3f = "=fff"

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
                f.write(struct.pack(mod_f, face.vert[i]))
                if len(self.texcoords):
                    f.write(struct.pack(mod_f, face.tex[i]))
                if len(self.normals):
                    f.write(struct.pack(mod_f, face.norm[i]))

        f.close()
        
        return

if __name__ == "__main__":
    m = Model()
    m.LoadOBJ("../.notes/teapot.obj")
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
    print "Size:\n\tVerts:{0}\n\tUVs:{1}\n\tNormals:{2}\n\tPolys:{3}\n".format(len(m.vertices),
                                                                               len(m.texcoords),
                                                                               len(m.normals),
                                                                               len(m.faces))
    m.SaveMesh("../.notes/test2.mesh")

