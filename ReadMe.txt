Author:
    Tashwin Khurana

To Build:
    >>qmake matrix_ui.pro

    >>make

To Cleanup:
    >>make clean

To Launch application:
    >>./matrix_ui

Requirement:
    QT version: 5.5 

Platform:
    Ubuntu

Description:
    A Qt-based user interface following characteristics:
    
        • Can load csv files delimited by commas, display a table with a large matrix (> 20K by 20K) of double-precision numbers.
        
        • Allows you to edit this matrix, save it to disk, and load it back again
        
        • Cells that you change should be highlighted yellow until you save -- then displayed with a white background.
        
        • Designed using a .ui file
        
        • Loads multiple matrices into separate tables simultaneously, allows comparison and edits on both.
        
        • Compares the multiple matrices and highlights the differences.

Widget Descriptions:
    Each Matrix has following associated use:
        Upload: To upload csv file
        Save: save changes to file
        Next/Prev: to load next/prev chunk of data into table

    Load Multiple check box allows user to load multiple files at a time

    Close after saving: Check box allows user to close the file after saving
            (Recommended to be enabled for extremely large file or it might take a couple of minutes to save file and read it again)

    Edit/Compare: Allows you to select whether you want to edit or compare the data between two matrices


Constrains:
    Loading in and writing to disk for larger files is a little slow
    Testing was not complete due to time constraints
    Error checking not complete
    Assumes that user is providing correct data
    Documentation is Sparse But have tried to docuent on a higher level

Features:
    Fast editing
    Fast comparison between matrices
    Loads file in at constant row intevals instead of complete files
    Handles any dimension matrix
    Tested with Files upto 4Gb

Test Samples:
    3 files of varied sizes for minimal testing
	(Did not include larger file here)

Reference:
    Qt Documentation
    Qt Model/View Tutorial
