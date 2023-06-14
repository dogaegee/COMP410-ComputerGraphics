using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class SpawnManager : MonoBehaviour
{
    // Singleton instance
    private static SpawnManager instance;



    // Public property to access the instance
    public static SpawnManager Instance
    {
        get
        {
            // If the instance is null, find it in the scene
            if (instance == null)
            {
                instance = FindObjectOfType<SpawnManager>();

                // If still null, create a new SpawnManager object
                if (instance == null)
                {
                    GameObject obj = new GameObject("SpawnManager");
                    instance = obj.AddComponent<SpawnManager>();
                }
            }
            return instance;
        }
    }

    public Camera mainCamera;
    public List<GameObject> asteroids;
    public float spawnRange = 10f;
    public float moveSpeed = 5f;
    public float spawnDistance = 100f;

    public List<GameObject> attackersOnScreen;
    private void Awake()
    {
        // Ensure only one instance of the SpawnManager exists
        if (instance != null && instance != this)
        {
            Destroy(gameObject);
            return;
        }

        instance = this;
        DontDestroyOnLoad(gameObject);
    }

    void Start()
    {

        if (mainCamera == null)
        {
            mainCamera = Camera.main;
        }

        InvokeRepeating(nameof(Spawn), 5f, 1f);
    }

    void Spawn()
    {
        if (ScoreManager.Instance.GetScore() <= 0)
        {
            moveSpeed = 40f;
            return;
        }
            GameObject currentAsteroid = asteroids[Random.Range(0, asteroids.Count)];
            Vector3 randomPosition = SpawnObjectOutsideCamera();
            GameObject newAsteroid = Instantiate(currentAsteroid, randomPosition, Quaternion.identity);
            StartCoroutine(MoveTowardsOrigin(newAsteroid));
            attackersOnScreen.Add(newAsteroid);
    }

    IEnumerator MoveTowardsOrigin(GameObject obj)
    {
        while (obj != null)
        {
            Vector3 direction = Vector3.Normalize(Vector3.zero - obj.transform.position);
            obj.transform.position += direction * moveSpeed * Time.deltaTime;
            yield return null;
        }
    }


    private Vector3 SpawnObjectOutsideCamera()
    {
        // Calculate the spawn position
        Vector3 spawnPosition = Vector3.zero;

        // Get the camera's viewport dimensions
        float viewportHeight = mainCamera.orthographicSize * 2f;
        float viewportWidth = viewportHeight * mainCamera.aspect;

        // Calculate the spawn position outside the camera's view
        float buffer = 1.5f; // Adjust this value as needed to ensure the object is fully outside the camera
        float threshold = 3f;

        float spawnX = Random.Range(-viewportWidth * buffer, viewportWidth * buffer);
        while (spawnX > -threshold && spawnX < threshold)
        {
            spawnX = Random.Range(-viewportWidth * buffer, viewportWidth * buffer);
        }

        float spawnY = Random.Range(-viewportHeight * buffer, viewportHeight * buffer);
        while (spawnY > -threshold && spawnY < threshold)
        {
            spawnY = Random.Range(-viewportHeight * buffer, viewportHeight * buffer);
        }

        float spawnZ = Random.Range(-viewportWidth * buffer, viewportWidth * buffer);
        while (spawnZ > -threshold && spawnZ < threshold)
        {
            spawnZ = Random.Range(-viewportWidth * buffer, viewportWidth * buffer);
        }

        // Adjust the spawn position based on the camera's position and orientation
        spawnPosition.x = spawnX + mainCamera.transform.position.x;
        spawnPosition.y = spawnY + mainCamera.transform.position.y;
        spawnPosition.z = spawnZ + mainCamera.transform.position.z;

        return spawnPosition;
    }
}
