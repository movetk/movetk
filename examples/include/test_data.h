/*
 * Copyright (C) 2018-2020 HERE Europe B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 * License-Filename: LICENSE
 */

//
// Created by onur on 10/4/18.
//

#ifndef MOVETK_TEST_DATA_H
#define MOVETK_TEST_DATA_H

namespace testdata {

const char* c2d_raw_csv = "PROBE_ID,SAMPLE_DATE,LAT,LON,HEADING,SPEED,PROBE_DATA_PROVIDER\n"
                          "5ba036d622000bf923550ad1004d431,2018-09-18 00:59:52,34.1496845,-118.2323396,301.0,103.0,CONSUMER24\n"
                          "5b9ffc9922000a1f75170a67001e96f,2018-09-18 00:59:15,34.0853821,-118.4077053,42.0,54.0,CONSUMER24\n"
                          "5ba0494322000bf923550ace004d40f,2018-09-18 00:59:54,34.0707958,-117.9613911,41.0,48.0,CONSUMER24\n"
                          "5ba04d5922000bf923550ad1004d467,2018-09-18 00:59:50,34.0924457,-118.3385855,359.0,1.0,CONSUMER24\n"
                          "5ba04d5922000bf923550ad1004d467,2018-09-18 00:59:45,34.0922132,-118.338586,0.0,38.0,CONSUMER24\n"
                          "5ba0493422000a1f75170a66001ea3b,2018-09-18 00:59:41,34.1317176,-118.0518098,92.0,69.0,CONSUMER24\n"
                          "5ba0493422000a1f75170a66001ea3b,2018-09-18 00:59:36,34.1317091,-118.0528416,87.0,79.0,CONSUMER24\n"
                          "5ba01fcb22000bf923550acf004d43d,2018-09-18 00:59:35,33.987879,-118.4599364,109.0,0.0,CONSUMER24\n"
                          "5ba01fcb22000bf923550acf004d43d,2018-09-18 00:59:30,33.987879,-118.4599364,109.0,0.0,CONSUMER24\n";

const char* c2d_format_geolife_csv = "PROBE_ID,SAMPLE_DATE,LAT,LON,HEADING,SPEED,PROBE_DATA_PROVIDER\n"
"20090119002851.plt,2009-01-19 00:28:51,39.902993,116.386834,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:28:53,39.903464,116.386052,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:28:54,39.903335,116.386271,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:28:55,39.903173,116.386498,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:28:57,39.903118,116.38654,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:00,39.903079,116.386493,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:02,39.903003,116.386579,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:04,39.902956,116.386662,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:06,39.902911,116.386709,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:11,39.902871,116.386696,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:14,39.902827,116.386734,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:17,39.902773,116.386772,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:19,39.902781,116.386682,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:22,39.902752,116.386638,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:25,39.902752,116.386561,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:27,39.902697,116.386614,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:30,39.90271,116.386549,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:31,39.902674,116.386596,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:33,39.90269,116.386529,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:37,39.902631,116.38654,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:39,39.902582,116.386566,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:44,39.902545,116.386535,0,0,GEOLIFE\n"
"20090119002851.plt,2009-01-19 00:29:49,39.90252,116.386502,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:30:58,40.049809,116.304047,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:34,39.991098,116.33062,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:35,39.991411,116.330832,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:36,39.991172,116.330687,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:37,39.991379,116.330881,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:38,39.991343,116.33085,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:39,39.991443,116.33094,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:40,39.991407,116.330919,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:41,39.991384,116.330891,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:42,39.991371,116.330913,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:46,39.991379,116.330899,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:51,39.991421,116.330849,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:54,39.991406,116.330784,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:57,39.991392,116.330712,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:50:59,39.99137,116.330697,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:51:00,39.991368,116.330661,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:51:04,39.991379,116.330592,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:51:08,39.991395,116.330525,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:51:11,39.991411,116.330459,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:51:15,39.991398,116.330403,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:51:18,39.991385,116.330337,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:51:21,39.991359,116.330294,0,0,GEOLIFE\n"
"20090221123058.plt,2009-02-21 12:51:24,39.991334,116.330251,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:32:53,39.899773,116.385689,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:32:54,39.899651,116.385654,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:32:55,39.899699,116.385548,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:32:56,39.899632,116.385527,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:32:57,39.899592,116.385481,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:32:59,39.899575,116.385387,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:03,39.899602,116.385316,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:06,39.899618,116.385339,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:08,39.899638,116.385347,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:10,39.89968,116.385341,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:11,39.899736,116.385347,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:15,39.899778,116.38536,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:18,39.899818,116.385371,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:21,39.899866,116.385369,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:25,39.899913,116.38535,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:30,39.89994,116.385368,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:34,39.900023,116.385414,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:36,39.900117,116.385483,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:37,39.900167,116.385499,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:38,39.900195,116.385553,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:39,39.90023,116.385586,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:41,39.900298,116.385639,0,0,GEOLIFE\n"
"20090204043253.plt,2009-02-04 04:33:43,39.900343,116.385672,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:45:23,39.898394,116.38643,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:45:28,39.898934,116.385613,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:45:33,39.898788,116.38576,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:45:38,39.898791,116.385717,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:45:43,39.898863,116.385583,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:45:48,39.898915,116.385459,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:45:53,39.898901,116.385441,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:45:58,39.898954,116.385343,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:46:03,39.898941,116.385362,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:46:08,39.898786,116.385663,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:46:13,39.898697,116.385845,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:46:18,39.898587,116.386024,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:46:23,39.898489,116.386203,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:46:28,39.898403,116.386375,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:46:33,39.898342,116.386464,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:46:38,39.89821,116.386592,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:46:43,39.898245,116.386527,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:46:48,39.898286,116.386429,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:46:53,39.898343,116.38636,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:46:58,39.898392,116.386281,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:47:03,39.898555,116.386065,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:47:08,39.899066,116.38532,0,0,GEOLIFE\n"
"20081223004523.plt,2008-12-23 00:47:13,39.898978,116.385465,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:04,39.900254,116.385889,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:05,39.900255,116.385475,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:06,39.900253,116.385424,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:07,39.900231,116.385502,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:08,39.90023,116.385554,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:10,39.900239,116.385607,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:11,39.900245,116.385657,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:14,39.900245,116.385657,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:16,39.900244,116.385656,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:21,39.900268,116.385673,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:26,39.900319,116.385696,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:28,39.900294,116.385616,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:30,39.900259,116.385568,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:32,39.900223,116.385519,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:34,39.900195,116.385442,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:36,39.900168,116.385374,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:40,39.900118,116.385346,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:43,39.900075,116.385329,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:46,39.900025,116.385313,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:49,39.899982,116.385295,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:52,39.899937,116.385294,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:55,39.899892,116.385299,0,0,GEOLIFE\n"
"20090218095704.plt,2009-02-18 09:57:58,39.899845,116.385309,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 03:58:16,39.999242,116.197588,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:01:23,40.002331,116.202158,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:01:24,40.002397,116.20234,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:01:25,40.002394,116.202325,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:06,40.003769,116.200072,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:07,40.003797,116.199966,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:08,40.003709,116.199877,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:09,40.003664,116.199861,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:11,40.003674,116.199829,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:12,40.003683,116.199814,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:13,40.003667,116.199892,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:15,40.003663,116.199856,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:16,40.003665,116.199964,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:17,40.003667,116.200005,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:18,40.00368,116.20007,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:19,40.003695,116.200137,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:21,40.003674,116.200096,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:24,40.003671,116.200108,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:29,40.003703,116.20012,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:31,40.003717,116.200138,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:34,40.003768,116.200153,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:37,40.003799,116.200161,0,0,GEOLIFE\n"
"20090322035816.plt,2009-03-22 04:20:39,40.003814,116.20017,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:15:33,39.900114,116.386047,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:15:35,39.900034,116.386164,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:15:36,39.900023,116.386223,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:15:39,39.900009,116.386284,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:15:40,39.899995,116.386341,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:15:41,39.899978,116.3864,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:15:45,39.899972,116.386476,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:15:49,39.899946,116.386402,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:15:53,39.899901,116.386419,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:15:58,39.899872,116.38647,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:16:00,39.899844,116.386519,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:16:03,39.899816,116.386466,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:16:08,39.899764,116.386492,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:16:13,39.899729,116.386435,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:16:16,39.899715,116.386366,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:16:18,39.899689,116.386287,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:16:20,39.899663,116.386232,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:16:23,39.899612,116.386187,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:16:27,39.899548,116.38619,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:16:30,39.89949,116.386223,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:16:33,39.899439,116.386213,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:16:38,39.899403,116.386247,0,0,GEOLIFE\n"
"20090318111533.plt,2009-03-18 11:16:43,39.899384,116.386239,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:33:00,39.926,116.3374,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:33:05,39.926009,116.337329,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:33:10,39.926,116.337306,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:33:15,39.925949,116.337308,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:33:20,39.925938,116.337286,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:33:25,39.925925,116.337247,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:33:30,39.925956,116.337181,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:33:35,39.925977,116.337127,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:33:40,39.92599,116.337031,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:33:45,39.926047,116.336932,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:33:50,39.926107,116.336848,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:33:55,39.926185,116.336779,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:34:00,39.926239,116.336701,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:34:05,39.926312,116.336661,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:34:10,39.92636,116.336614,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:34:15,39.926391,116.336536,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:34:20,39.926403,116.336484,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:34:25,39.926421,116.336466,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:34:30,39.926435,116.336455,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:34:35,39.926395,116.336435,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:34:40,39.926388,116.336396,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:34:45,39.92642,116.33656,0,0,GEOLIFE\n"
"20081122023300.plt,2008-11-22 02:34:50,39.926419,116.336577,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:08:29,39.926106,116.337191,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:08:31,39.926127,116.337108,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:08:32,39.926241,116.337103,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:08:37,39.926183,116.33718,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:08:42,39.926231,116.337173,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:08:47,39.92619,116.33712,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:08:52,39.926114,116.337117,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:08:55,39.926114,116.337117,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:08:57,39.926108,116.33711,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:09:02,39.926078,116.337,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:09:07,39.926094,116.336978,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:09:11,39.926102,116.337051,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:09:16,39.926167,116.337024,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:09:21,39.926242,116.336996,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:09:26,39.9263,116.33695,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:09:31,39.926358,116.336914,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:09:36,39.92641,116.336909,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:09:41,39.926461,116.336911,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:09:46,39.926489,116.336945,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:09:47,39.92677,116.337078,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:09:50,39.92686,116.337036,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:09:55,39.926931,116.337108,0,0,GEOLIFE\n"
"20081031000829.plt,2008-10-31 00:10:00,39.926943,116.337111,0,0,GEOLIFE\n"
"20090123120357.plt,2009-01-23 12:03:57,39.876418,116.304274,0,0,GEOLIFE\n"
"20090123120357.plt,2009-01-23 12:03:58,39.876273,116.304401,0,0,GEOLIFE\n"
"20090123120357.plt,2009-01-23 12:03:59,39.876393,116.304264,0,0,GEOLIFE\n"
"20090123120357.plt,2009-01-23 12:04:00,39.876341,116.304284,0,0,GEOLIFE\n"
;

//const char* c2d_format_geolife_raw_csv =

const char* geolife_raw_csv = "LAT,LONG,ALTITUDE,DATETIME,TRAJID,MODEID\n"
                              "39.894178,116.3182,-777,1206712480,010_39,train\n"
                              "39.894505,116.321132,-777,1206712514,010_39,train\n"
                              "39.894953,116.326452,-777,1206712573,010_39,train\n"
                              "39.8946,116.332542,-777,1206712632,010_39,train\n"
                              "39.889622,116.33704,-777,1206712691,010_39,train\n"
                              "39.88209,116.338353,-777,1206712750,010_39,train\n"
                              "39.873873,116.338455,-777,1206712810,010_39,train\n"
                              "39.865182,116.338058,-777,1206712869,010_39,train\n"
                              "39.855403,116.33771,-777,1206712928,010_39,train\n"
                              "39.844532,116.334362,-777,1206712987,010_39,train\n"
                              "39.511005,116.700307,-777,1206716105,010_39,train\n"
                              "39.511005,116.700307,-777,1206716162,010_39,train\n"
                              "39.511005,116.700307,-777,1206716164,010_39,train\n"
                              "39.51097,116.700392,-777,1206716223,010_39,train\n"
                              "39.509652,116.702593,-777,1206716283,010_39,train\n"
                              "39.506952,116.707353,-777,1206716342,010_39,train\n"
                         //   "39.50293,116.714948,-777,1206716401,010_40,train\n"
                         //   "39.497045,116.726137,-777,1206716460,010_40,train\n"
                         //   "39.489695,116.740047,-777,1206716519,010_40,train\n"
                         //   "39.481438,116.755648,-777,1206716579,010_40,train\n"
                         //   "39.472748,116.770972,-777,1206716638,010_40,train\n"
                              ;
}

#endif //MOVETK_TEST_DATA_H