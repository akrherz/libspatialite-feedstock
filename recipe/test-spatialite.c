#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <spatialite.h>

int main(int argc, char **argv) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    void *cache = NULL;
    char *error = NULL;
    int rc = 1;
    const char *sql = "SELECT ST_X(MakePoint(12.5,-34.25,4326)), "
        "ST_Y(MakePoint(12.5,-34.25,4326)), HasGeos(), HasProj(), HasRtTopo(), "
        "ST_Equals(GeomFromText('POINT(1 2)',4326),MakePoint(1,2,4326))";
    if (argc < 3 || sqlite3_open(":memory:", &db) != SQLITE_OK) goto done;
    if (strcmp(argv[1], "module") == 0) {
        if (argc != 4 || sqlite3_enable_load_extension(db, 1) != SQLITE_OK) goto done;
        if (sqlite3_load_extension(db, argv[3], NULL, &error) != SQLITE_OK) {
            fprintf(stderr, "extension load: %s\n", error);
            sqlite3_free(error);
            goto done;
        }
    } else {
        cache = spatialite_alloc_connection();
        if (!cache) goto done;
        spatialite_init_ex(db, cache, 0);
    }
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) goto done;
    if (sqlite3_step(stmt) != SQLITE_ROW) goto done;
    if (sqlite3_column_double(stmt,0) != 12.5 || sqlite3_column_double(stmt,1) != -34.25
        || sqlite3_column_int(stmt,2) != 1 || sqlite3_column_int(stmt,3) != 1
        || sqlite3_column_int(stmt,4) != atoi(argv[2]) || sqlite3_column_int(stmt,5) != 1)
        goto done;
    printf("SpatiaLite %s geometry, GEOS, PROJ and license-feature checks passed\n", argv[1]);
    rc = 0;
done:
    if (rc) fprintf(stderr, "SpatiaLite consumer failed: %s\n", db ? sqlite3_errmsg(db) : "no database");
    sqlite3_finalize(stmt);
    if (db) sqlite3_close(db);
    if (cache) spatialite_cleanup_ex(cache);
    spatialite_shutdown();
    return rc;
}
