cmake_minimum_required(VERSION 3.17)

PROJECT(sqlite3 C)

add_library(sqlite3 STATIC sqlite3.c)