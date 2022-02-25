
/*CREATE DATABASE IF NOT EXISTS EmploieDuTemps CHARACTER SET 'utf8'*/

CREATE TABLE IF NOT EXISTS Niveau (
  idNiveau INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  nomNiveau VARCHAR(100),
  nomSalle VARCHAR(100),
  adder VARCHAR(100) DEFAULT CURRENT_USER,
  isDeleted BOOLEAN DEFAULT 0,
  UNIQUE KEY(nomNiveau)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS  MatiereStatique (
  idMatiere INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  nomMatiere VARCHAR(100) NOT NULL,
  idNiveau INT UNSIGNED NOT NULL,
  UNIQUE KEY(nomMatiere, idNiveau),
  FOREIGN KEY (idNiveau) REFERENCES Niveau(idNiveau)
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS  Enseignant (
  idEnseignant INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  nomEnseignant VARCHAR(100),
  prenomEnseignant VARCHAR(100),
  grade VARCHAR(10),
  isActived BOOLEAN DEFAULT 1
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS Emploie(
idEmploie INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
idNiveau INT UNSIGNED,
dateDebut DATE,
dateFin DATE,
dateCreation DATE DEFAULT CURRENT_DATE,
lieuCreation VARCHAR(90),
createur VARCHAR(90) DEFAULT CURRENT_USER,
signature LONGBLOB,
semestre VARCHAR(20),
UNIQUE KEY(DateDebut, DateFin, idNiveau),
FOREIGN KEY(idNiveau) REFERENCES Niveau(idNiveau)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS Horaire(
  idHoraire INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
  numCase INT UNSIGNED,
  heureDebut TIME,
  heureFin TIME,
  idEmploie INT UNSIGNED,
  FOREIGN KEY (idEmploie) REFERENCES Emploie(idEmploie),
  UNIQUE KEY(numCase, idEmploie,heureDebut, heureFin)
)ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS Matiere (
  idMatiere INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
  idMatiereStatique INT UNSIGNED,
  idEmploie INT UNSIGNED,
  numCase INT(2) NOT NULL,
  couleur VARCHAR(10),
  type_ VARCHAR(4),
  commentaire VARCHAR(90),
  UNIQUE KEY(idMatiereStatique, idEmploie, numCase),
  FOREIGN KEY (idMatiereStatique) REFERENCES MatiereStatique (idMatiere),
  FOREIGN KEY (idEmploie) REFERENCES Emploie(idEmploie)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS Enseigner (
  idMatiere INT UNSIGNED,
  idEnseignant INT UNSIGNED,
  dateDebut DATE DEFAULT CURRENT_DATE(),
  dateFin DATE,
  FOREIGN KEY(idMatiere) REFERENCES MatiereStatique(idMatiere),
  FOREIGN KEY(idEnseignant) REFERENCES Enseignant(idEnseignant),
  UNIQUE KEY(idEnseignant, idMatiere, dateDebut, dateFin)
)ENGINE=InnoDB;


CREATE OR REPLACE VIEW EmploiView AS SELECT DISTINCT Matiere.couleur, Matiere.idMatiereStatique, Matiere.numCase AS numeroCase,
Matiere.type_ AS typeMatiere, MatiereStatique.nomMatiere, Enseignant.nomEnseignant, Enseignant.prenomEnseignant,
Enseignant.grade, Niveau.nomNiveau AS niveau, Niveau.nomSalle, Matiere.idEmploie AS idEmploi, Matiere.commentaire, Emploie.dateDebut, 
Emploie.dateFin, Emploie.semestre, Emploie.dateCreation, Emploie.lieuCreation  
FROM Matiere LEFT JOIN Emploie ON Emploie.idEmploie = Matiere.idEmploie
                     LEFT JOIN MatiereStatique ON Matiere.idMatiereStatique = MatiereStatique.idMatiere  
                     LEFT JOIN Niveau ON MatiereStatique.idNiveau = Niveau.idNiveau
                     LEFT JOIN Enseigner ON Matiere.idMatiereStatique = Enseigner.idMatiere 
                     LEFT JOIN Enseignant ON Enseigner.idEnseignant = Enseignant.idEnseignant;


CREATE OR REPLACE VIEW MatiereStaticeView AS SELECT MatiereStatique.idMatiere, nomMatiere, Enseignant.idEnseignant, 
nomEnseignant, prenomEnseignant, grade, nomNiveau AS niveau FROM MatiereStatique 
LEFT JOIN Enseigner ON MatiereStatique.idMatiere = Enseigner.idMatiere 
LEFT JOIN Enseignant ON Enseignant.idEnseignant = Enseigner.idEnseignant LEFT JOIN Niveau 
ON MatiereStatique.idNiveau = Niveau.idNiveau;

CREATE OR REPLACE VIEW EnseignantView AS SELECT Enseignant.idEnseignant, nomEnseignant, prenomEnseignant, grade, 
nomMatiere, nomNiveau, Enseigner.dateDebut, Enseigner.DateFin, Enseignant.isActived 
FROM Enseignant 
LEFT JOIN Enseigner ON Enseignant.idEnseignant = Enseigner.idEnseignant
LEFT JOIN MatiereStatique ON Enseigner.idMatiere = MatiereStatique.idMatiere 
LEFT JOIN Niveau ON MatiereStatique.idNiveau = Niveau.idNiveau;








