server {
    host = '0.0.0.0'
    port = 8080
    ttl = 5 * 60 * 1000
    context = '/'

    session {
        ttl = 30 * 60
    }

    debugOutput = false
    onlyCachedHeaders = false
}
