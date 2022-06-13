#simple script to create dual yarn
textile = CTextile()
yarn = CDualYarn()

yarn.AddNode(CNode(XYZ(0, 0, 0)))
yarn.AddNode(CNode(XYZ(5, 0, 1)))
yarn.AddNode(CNode(XYZ(10, 0, 0)))

textile.AddDualYarn(yarn)
AddTextile("dual yarn",textile)
#GetRenderWindow('dual yarn').SetXRay(True)
#GetRenderWindow('dual yarn').RemoveTextiles()
#GetRenderWindow('dual yarn').RenderTextile('dual yarn')

#GetRenderWindow('dual yarn').RenderNodes('dual yarn')
#GetRenderWindow('dual yarn').RenderPaths('dual yarn')
#GetRenderWindow('dual yarn').RenderMesh('dual yarn')