using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
    // Singleton instance
    private static GameManager instance;

    // Public property to access the instance
    public static GameManager Instance
    {
        get
        {
            // If the instance is null, find it in the scene
            if (instance == null)
            {
                instance = FindObjectOfType<GameManager>();

                // If still null, create a new GameManager object
                if (instance == null)
                {
                    GameObject obj = new GameObject("GameManager");
                    instance = obj.AddComponent<GameManager>();
                }
            }
            return instance;
        }
    }


    private void Awake()
    {
        // Ensure only one instance of the GameManager exists
        if (instance != null && instance != this)
        {
            Destroy(gameObject);
            return;
        }

        instance = this;
        DontDestroyOnLoad(gameObject);
    }

    // Start is called before the first frame update
    void Start()
    {
        // Initialization code
    }

    // Update is called once per frame
    void Update()
    {
        // Update logic
    }
}
