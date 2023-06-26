import vtk

def writeObjects(nodeCoords,
                 edges = [],
                 scalar = [], name = '', power = 1,
                 scalar2 = [], name2 = '', power2 = 1,
                 nodeLabel = [],
                 method = 'vtkPolyData',
                 fileout = 'test'):
    """
    Store points and/or graphs as vtkPolyData or vtkUnstructuredGrid.
    Required argument:
    - nodeCoords is a list of node coordinates in the format [x,y,z]
    Optional arguments:
    - edges is a list of edges in the format [nodeID1,nodeID2]
    - scalar/scalar2 is the list of scalars for each node
    - name/name2 is the scalar's name
    - power/power2 = 1 for r~scalars, 0.333 for V~scalars
    - nodeLabel is a list of node labels
    - method = 'vtkPolyData' or 'vtkUnstructuredGrid'
    - fileout is the output file name (will be given .vtp or .vtu extension)
    """

    points = vtk.vtkPoints()
    for node in nodeCoords:
        points.InsertNextPoint(node)

    if edges:
        line = vtk.vtkCellArray()
        line.Allocate(len(edges))
        for edge in edges:
            line.InsertNextCell(2)
            line.InsertCellPoint(edge[0])
            line.InsertCellPoint(edge[1])   # line from point edge[0] to point edge[1]

    if scalar:
        attribute = vtk.vtkFloatArray()
        attribute.SetNumberOfComponents(1)
        attribute.SetName(name)
        attribute.SetNumberOfTuples(len(scalar))
        for i, j in enumerate(scalar):   # i becomes 0,1,2,..., and j runs through scalars
            attribute.SetValue(i,j**power)

    if scalar2:
        attribute2 = vtk.vtkFloatArray()
        attribute2.SetNumberOfComponents(1)
        attribute2.SetName(name2)
        attribute2.SetNumberOfTuples(len(scalar2))
        for i, j in enumerate(scalar2):   # i becomes 0,1,2,..., and j runs through scalar2
            attribute2.SetValue(i,j**power2)

    if nodeLabel:
        label = vtk.vtkFloatArray()     #label = vtk.vtkStringArray()
        label.SetName('tag')
        label.SetNumberOfValues(len(nodeLabel))
        for i, j in enumerate(nodeLabel):   # i becomes 0,1,2,..., and j runs through scalar
            label.SetValue(i,j)

    if method == 'vtkPolyData':
        polydata = vtk.vtkPolyData()
        polydata.SetPoints(points)
        if edges:
            polydata.SetLines(line)
        if scalar:
            polydata.GetPointData().AddArray(attribute)
        if scalar2:
            polydata.GetPointData().AddArray(attribute2)
        if nodeLabel:
            polydata.GetPointData().AddArray(label)
        writer = vtk.vtkXMLPolyDataWriter()
        writer.SetFileName(fileout+'.vtp')
        writer.SetInputData(polydata)
        writer.Write()
    elif method == 'vtkUnstructuredGrid':
        # caution: ParaView's Tube filter does not work on vtkUnstructuredGrid
        grid = vtk.vtkUnstructuredGrid()
        grid.SetPoints(points)
        if edges:
            grid.SetCells(vtk.VTK_LINE, line)
        if scalar:
            grid.GetPointData().AddArray(attribute)
        if scalar2:
            grid.GetPointData().AddArray(attribute2)
        if nodeLabel:
            grid.GetPointData().AddArray(label)
        writer = vtk.vtkXMLUnstructuredGridWriter()
        writer.SetFileName(fileout+'.vtu')
        writer.SetInputData(grid)
        writer.Write()
