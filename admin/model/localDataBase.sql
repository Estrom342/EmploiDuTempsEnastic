CREATE TABLE IF NOT EXISTS Niveau(
        idNiveau INTEGER PRIMARY KEY AUTOINCREMENT,
        nomNiveau VARCHAR(90)
);
CREATE TABLE IF NOT EXISTS Emploi(
        idEmploi INTEGER PRIMARY KEY AUTOINCREMENT,
        idNiveau INTEGER,
        dateDebut DATE,
        dateFin DATE,
        CONSTRAINT fkIdNiveau FOREIGN KEY (idNiveau) REFERENCES Niveau(idNiveau)
);
CREATE TABLE IF NOT EXISTS Matiere(
        idMatiere INTEGER PRIMARY KEY AUTOINCREMENT,
        idEmploi INTEGER,
        type VARCHAR(10),
        nomMatiere VARCHAR(100),
        nomEnseignant VARCHAR(100),
        couleur VARCHAR(20),
        numCase INTEGER,
        CONSTRAINT fkIdEmploi FOREIGN KEY(idEmploi) REFERENCES Emploi(idEmploi)
);
CREATE TABLE IF NOT EXISTS Parametre(
        hostName VARCHAR(90),
        portName INTEGER,
        studentName VARCHAR(90),
        studentLevel VARCHAR(90),
        password VARCHAR(90),
        theme VARCHAR(90),
        signature LONGBLOB
);

DROP VIEW IF EXISTS EmploiView;

CREATE VIEW EmploiView AS SELECT Emploi.dateDebut, Emploi.dateFin, Niveau.nomNiveau,
Matiere.type AS typeMatiere, Matiere.nomMatiere, Matiere.nomEnseignant, Matiere.couleur, Matiere.numCase  
FROM Emploi LEFT JOIN Niveau ON Emploi.idNiveau = Niveau.idNiveau LEFT JOIN Matiere ON Emploi.idEmploi = Matiere.idEmploi;
