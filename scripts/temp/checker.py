
arrs = [
"{6,1},{7,5},{2,0},{3,4}",
"{6,1},{2,5},{0,7},{3,4}",
"{6,1},{3,7},{4},{5,2,0});",
"{6,1},{5,7},{},{0,2,4,3}",
"{6,1},{0,5},{7,2},{3,4}",
"{4,1},{6,7},{3},{0,2,5}",
"{4,1},{2,5},{0,7},{3,6}",
"{4,1},{3,5},{0,2},{6,7}",
"{4,1},{7,5},{0,2},{3,6}",
"{4,1},{0,5},{2,3},{6,7}",
"{5,2},{4,6},{1,3},{0,7}",
"{5,2},{0,4},{1,6},{3,7}",
"{5,2},{1,6},{3,4},{0,7}",
"{5,2},{6,4},{3,1},{7,0}",
"{5,2},{3,6},{4,1},{0,7}",
"{7,5},{0,2},{1,4,6},{3}",
"{7,5},{4,2},{1,6,0},{3}",
"{7,5},{2,3},{1,6,4},{0}",
"{7,5},{6,1},{2,0},{3,4}",
"{7,5},{3,1},{0,2},{4,6}",
]

for arr in arrs:
    myset = set([s for s in arr.replace("{","").replace("}","").split(",") if s!=""])
    print(len(myset))