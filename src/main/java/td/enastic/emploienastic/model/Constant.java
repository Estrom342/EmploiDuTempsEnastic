

package td.enastic.emploienastic.model;
public class Constant {

    public final static String DATABASE_NAME = "data.sql";
    public final static String DEFAULT_HOSTNAME = "emploi.enastic.lan";
    public final static int    DEFAULT_PORT_NAME = 2222;
    public final static String FLAG = "FLAG";
    public final static String GET_EMPLOI_FLAG = "GET_EMPLOI_FLAG";
    public final static String EMPLOI_FLAG = "EMPLOI_FLAG";
    public final static String GET_LAST_EMPLOI_FLAG = "GET_LAST_EMPLOI_FLAG";
    public final static String LAST_EMPLOI_FLAG = "LAST_EMPLOI_FLAG";
    public final static String SIGNATURE_EMPLOI_FLAG = "SIGNATURE_EMPLOI_FLAG";
    public final static String GET_SIGNATURE_EMPLOI_FLAG = "GET_SIGNATURE_EMPLOI_FLAG";
    public final static String PALETTE_CLEAR_TXT = "Clear";
    public final static String PALETTE_SOMBRE_TXT = "Sombre";
    public final static String DEFAULT_COMMENT = "DEFAULT_COMMENT";
    public final static String SIGNATURE_VIDE = "SIGNATURE_VIDE";
    public final static String MAIN_SOCKET_NAME = "MAIN_SOCKET_NAME";
    public final static String SIGNATURE_SOCKET_NAME = "SIGNATURE_SOCKET_NAME";
    public final static String TIME_TABLE_SOCKET_NAME = "TIME_TABLE_SOCKET_NAME";
    public final static String ASK_TIMETABLE_SOCKET_NAME = "ASK_TIMETABLE_SOCKET_NAME";
    public final static String TEMP_SOCKET_NAME = "TEMP_SOCKET_NAME";
    public final static String DEFAULT_SIGNATURE = "DEFAULT_SIGNATURE";

    public final static String OFFLINE_ICON     = "⬤";
    public final static String ONLINE_ICON      = "🔵";

    public final static String ON_APDATE_DATABASE = "DROP DATABASE IF EXISTS "+DATABASE_NAME;
    public final static String DATABASE_ON_CREATE =
            "CREATE TABLE IF NOT EXISTS Niveau(\n" +
                    "        idNiveau INTEGER PRIMARY KEY AUTOINCREMENT,\n" +
                    "        nomNiveau VARCHAR(90),\n" +
                    "        nomSalle VARCHAR(100)\n" +
                    ");\n" +
                    "CREATE TABLE IF NOT EXISTS Emploi(\n" +
                    "        idEmploi INTEGER PRIMARY KEY AUTOINCREMENT,\n" +
                    "        idNiveau INTEGER,\n" +
                    "        dateDebut DATE,\n" +
                    "        dateFin DATE,\n" +
                    "        dateCreation DATE DEFAULT CURRENT_DATE,\n" +
                    "        lieuCreation VARCHAR(90),\n" +
                    "        semestre VARCHAR(20),\n" +
                    "        signature LONGBLOG,\n" +
                    "        CONSTRAINT fkIdNiveau FOREIGN KEY (idNiveau) REFERENCES Niveau(idNiveau)\n" +
                    ");\n" +
                    "CREATE TABLE IF NOT EXISTS Horaire(\n" +
                    "  idHoraire INTEGER PRIMARY KEY AUTOINCREMENT,\n" +
                    "  numCase INTEGER,\n" +
                    "  heureDebut TIME,\n" +
                    "  heureFin TIME,\n" +
                    "  idEmploi INTEGER,\n" +
                    "  CONSTRAINT fkIdEmploi FOREIGN KEY (idEmploi) REFERENCES Emploi(idEmploi)\n" +
                    ");\n" +
                    "CREATE TABLE IF NOT EXISTS Matiere(\n" +
                    "        idMatiere INTEGER PRIMARY KEY AUTOINCREMENT,\n" +
                    "        idEmploi INTEGER,\n" +
                    "        type VARCHAR(10),\n" +
                    "        nomMatiere VARCHAR(100),\n" +
                    "        nomEnseignant VARCHAR(100),\n" +
                    "        couleur VARCHAR(20),\n" +
                    "        numCase INTEGER,\n" +
                    "         commentaire VARCHAR(100),\n" +
                    "        CONSTRAINT fkIdEmploi FOREIGN KEY(idEmploi) REFERENCES Emploi(idEmploi)\n" +
                    ");\n" +
                    "CREATE TABLE IF NOT EXISTS Parametre(\n" +
                    "        hostName VARCHAR(90),\n" +
                    "        portName INTEGER,\n" +
                    "        studentName VARCHAR(90),\n" +
                    "        studentLevel VARCHAR(90),\n" +
                    "        password VARCHAR(90),\n" +
                    "        theme VARCHAR(90),\n" +
                    "        signature LONGBLOB\n" +
                    ");\n" +
                    "DROP VIEW IF EXISTS EmploiView;\n" +
                    "CREATE VIEW EmploiView AS SELECT Emploi.idEmploi, Emploi.dateDebut, Emploi.dateFin, Niveau.nomNiveau, \n" +
                    "Niveau.nomSalle, Emploi.dateCreation, Emploi.lieuCreation, Emploi.semestre, Emploi.signature, Matiere.type AS typeMatiere, \n" +
                    "Matiere.nomMatiere, Matiere.nomEnseignant, Matiere.couleur, Matiere.numCase, Matiere.commentaire\n" +
                    "FROM Emploi LEFT JOIN Niveau ON Emploi.idNiveau = Niveau.idNiveau LEFT JOIN Matiere ON Emploi.idEmploi = Matiere.idEmploi;\n";


    public static String HTML_HEADER = "<!DOCTYPE html>\n" +
            "<html>" +
            "<head>" +
            "<meta charset=\"utf-8\">" +
            "</head>" +
            "<body>";
    public static String HTML_FOOTER = "</body></html>";

    public final static String HELP_TEXT =
            "<body style='background: rgb(67, 22, 55); color:white; text-align: justify;'>\n" +
                    "<h2 style='color: rgb(74, 118, 225); text-decoration:underline; text-align:center;'>Aide d'utilisateur</h2>\n" +
                    "\n" +
                    "<h3 style='color: rgb(74, 118, 225); text-align: center;'>Emploie Enastic</h3>\n" +
                    "<p>\n" +
                    "        Emploi Enastic est une simple Applications de visualisations d’emploi du temps de votre classe.\n" +
                    "        Vous pouvez l'utilisez pour consulter l'emploi de temps de votre classe ou d'une autre classe.\n" +
                    "        Son autre fonctionnalité d'impression d’emplois vous permet d'imprimer votre emploi de temps parmis vos fichiers locaux pour les consulter en dehors de l’ applications.\n" +
                    "</p>\n" +
                    "<h3 style='color: rgb(74, 118, 225); text-align: center;'>Bouton parametré</h3>\n" +
                    "<p>\n" +
                    "Le bouton paramètre permet de configurer le programme pour qu'il se connecte automatiquement à chaque demarrage au serveur d'emploi.<br/>\n" +
                    "Une fois clicquer dessus, une fenêtre de paramétrage s'affiche avec cinq(5) champs.<br/>\n" +
                    "<ul>\n" +
                    "        <li>\n" +
                    "                Le premier champ concerne l'adresse du seveur. En temps normal vous n'aurez pas besoin de le modifier. Mais s'il arrive que l'adresse du serveur soit changé, demander à l'administration la bonne adresse.\n" +
                    "        </li>\n" +
                    "        <li>\n" +
                    "                Le second champ concerne le numéro de port d'écoute du serveur d'emplois. Ne le modifiez pas a moins que l'administration ne vous demande de le faire. Par défaut il est réglé sur le port <strong>2222</strong>.\n" +
                    "        </li>\n" +
                    "        <li>\n" +
                    "                Le troisième champ concerne votre filière. Cela permettra au programme de vous afficher automatiquement l'emploi de temps de votre salle après avoir téléchargé tous les emplois.\n" +
                    "        </li>\n" +
                    "        <li>\n" +
                    "                Le quatrième champs permet de spécifier votre nom d'utilisateur, ce champ n'as aucun rôle particulier.\n" +
                    "        </li>\n" +
                    "        <li>\n" +
                    "                Le dernier vous permet de changer le thème de votre application. Il existe deux thèmes. Sombre et Clair, ce dernier est configuré par défaut à l'installation.\n" +
                    "        </li>\n" +
                    "</ul>\n" +
                    "\n" +
                    "<h3 style='color: rgb(74, 118, 225); text-align: center;'>Comment consulter un nouveau emploi</h3>\n" +
                    "\n" +
                    "<p>\n" +
                    "        Une fois que Emploi Enastic est démarré, vous pouvez consulter les nouveaux emplois de temps en vous connectant au réseau local de l'Enastic.<br/>\n" +
                    "        Pour voir un emploi particulier, faites un clic droit pour voir la liste des classes, ensuite selectionner la salle dont vous voulez consulter l'emploi.\n" +
                    "</p>\n" +
                    "\n" +
                    "<h3 style='color: rgb(74, 118, 225); text-align: center;'>Comment imprimer un emploi</h3>\n" +
                    "\n" +
                    "<p>\n" +
                    "        Pour imprimer un emploi de temps, clicquez tout simplement sur le bouton imprimer qui se trouve sur la barre d'outils. A ce moment une nouvelle fenêtre se présenteras a\n" +
                    "        vous, choisissez l'emplacement ou vous voulez enregistrer et cliquer sur imprimer.\n" +
                    "        Vous avez aussi la possibilité de modifier le nom de sortie de votre fichier.\n" +
                    "</p>\n" +
                    "\n" +
                    "</body>\n";
    public final static String TEXT_ABOUT=
            "<div style=\"boder-raduis: 15px; border:solid 1px black;\">"+
            "<h3 style='text-align:center; color:#4A76E1;'>Empoi Enastic version beta</h3>"+
            "<p style='text-align:center;'>Consulter votre emploi depuis votre ordinateur ou téléphone</p>" +
            "<h4 style='color: rgb(0, 0, 255);'>Crédit</h4>" +
            "<ul>" +
                "<h3>Equipe de developpement</h3>" +
                    "<ul>" +
                        "<li><a href='mailto:estromganda@gmail.com'>Ganda Estrom</a></li>" +
                        "<li><a href='mailto:mbaleiranezechiel@gmail.com'>Mba Le Iran Ezechiel</a></li>" +
                    "<ul>" +
            "<ul></div>";

    public final static String TIMETABLE_NOT_AVAIBLE_TEXT =
            "<div style=\"text-align: center; color: blue; font-size: 45px; font-style: italic;\">" +
                    "Aucun emploi du temps disponible</div>";
}

