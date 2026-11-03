/*M!999999\- enable the sandbox mode */ 
-- MariaDB dump 10.19-11.7.2-MariaDB, for osx10.20 (arm64)
--
-- Host: localhost    Database: agenda
-- ------------------------------------------------------
-- Server version	11.7.2-MariaDB

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*M!100616 SET @OLD_NOTE_VERBOSITY=@@NOTE_VERBOSITY, NOTE_VERBOSITY=0 */;

--
-- Table structure for table `contatos`
--

DROP TABLE IF EXISTS `contatos`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `contatos` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `nome` varchar(90) NOT NULL,
  `fone` varchar(15) NOT NULL,
  `email` varchar(60) NOT NULL,
  `data_criado` timestamp NULL DEFAULT current_timestamp(),
  `data_editado` timestamp NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`id`),
  UNIQUE KEY `email` (`email`)
) ENGINE=InnoDB AUTO_INCREMENT=26 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `contatos`
--

LOCK TABLES `contatos` WRITE;
/*!40000 ALTER TABLE `contatos` DISABLE KEYS */;
INSERT INTO `contatos` VALUES
(1,'Paulo Carneiro Pereira','(31) 98407-4079','p.pereira@gmail.com','2026-05-18 22:31:06','2026-11-03 19:57:31'),
(2,'Maria do Carmo Barbosa','(31) 99455-2331','maria.carmo@hotmail.com','2026-05-18 22:34:36','2026-10-30 04:40:30'),
(3,'José Roberto da Silva','(41) 98665-4159','jz.silva@gmail.com','2026-05-18 22:34:36','2026-11-03 19:58:01'),
(6,'Mariana Costa Pereira','(31) 99344-5566','mari_ferreira@yahoo.com.br','2026-05-18 22:36:42','2026-05-25 23:34:13'),
(9,'Lucas Fernando Simões','(19) 98776-3322','lucas.lima@live.com','2026-05-18 22:36:42','2026-05-25 23:33:33'),
(10,'Camila Oliveira Santos','(81) 99445-6677','camila.santos@gmail.com','2026-05-18 22:36:42','2026-05-18 22:36:42'),
(11,'Bruno Henrique Mendes','(48) 98227-9912','bruno.guedes@outlook.com.br','2026-05-18 22:36:42','2026-10-30 04:40:48'),
(12,'Larissa Martins Vieira','(62) 99118-4455','larissa.martins@yahoo.com','2026-05-18 22:36:42','2026-05-18 22:36:42'),
(16,'Pedro Santos Pereira','(51) 96512-4784','pedro.spereira@gmail.com','2026-05-18 22:43:55','2026-05-18 22:49:33'),
(17,'Paula Lima Ortega','(51) 6547-8856','paula@gmail.com','2026-05-25 22:47:57','2026-05-25 23:21:01'),
(19,'Sergio de Oliveira','(31) 2564-5580','s.oliva@gmail.com','2026-05-25 22:49:02','2026-05-25 22:49:02'),
(20,'Maria de Jesus','(61) 2564-8874','mj@outlook.com','2026-05-25 22:52:49','2026-05-25 22:52:49'),
(21,'Giovane de Souza','(44) 547-635895','giovane.s@gmail.com','2026-05-25 22:54:08','2026-05-25 22:54:08');
/*!40000 ALTER TABLE `contatos` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*M!100616 SET NOTE_VERBOSITY=@OLD_NOTE_VERBOSITY */;

-- Dump completed on 2026-11-03 18:05:29
