using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class R_Controller : MonoBehaviour
{
    [Header("Movement Settings")]
    [SerializeField] private Transform[] waypoints; // pontos para percorrer
    [SerializeField] private float speed = 3.0f;     // velocidade
    [SerializeField] private float waitTime = 2.0f;  // tempo de parada entre pontos

    private int currentWaypointIndex = 0;
    private bool isWaiting = false;
    private bool pathCompleted = false; 

    void Update()
    {
        // parar quando estiver esperando ou ter percorrido todos os pontos
	if (pathCompleted || isWaiting || waypoints.Length == 0) return;

        MoveTowardsWaypoint();
    }

    void MoveTowardsWaypoint()
    {
        // posição do alvo
        Transform targetWaypoint = waypoints[currentWaypointIndex];

        // Move o objeto em direção ao alvo
        transform.position = Vector3.MoveTowards(transform.position, targetWaypoint.position, speed * Time.deltaTime);

        // rotaciona o robo para a direção do alvo
        Vector3 direction = targetWaypoint.position - transform.position;
        if (direction != Vector3.zero)
        {
            Quaternion targetRotation = Quaternion.LookRotation(direction);
            transform.rotation = Quaternion.Slerp(transform.rotation, targetRotation, speed * 2 * Time.deltaTime);
        }

	if (Vector3.Distance(transform.position, targetWaypoint.position) < 0.1f)
        {
            // caminho completo se todos os pontos forem percorridos
            if (currentWaypointIndex >= waypoints.Length - 1)
            {
                pathCompleted = true;
                Debug.Log("Todos os pontos percorridos");
            }
            else
            {
                // continua para o proximo ponto se houver
                StartCoroutine(WaitAtWaypoint());
            }
        }
    }

    IEnumerator WaitAtWaypoint()
    {
        isWaiting = true;

        yield return new WaitForSeconds(waitTime);

        // itera o proximo ponto
        currentWaypointIndex++;

        isWaiting = false;
    }

    // vizualisação dos pontos no editor 
	private void OnDrawGizmos()
	{
	    // null check
	    if (waypoints == null || waypoints.Length == 0) return;

	    Gizmos.color = Color.yellow;
	    
	    for (int i = 0; i < waypoints.Length; i++)
	    {
		if (waypoints[i] != null)
		{
		    // esfera no lugar do ponto
		    Gizmos.DrawSphere(waypoints[i].position, 0.3f);

		    // editor bug fix - only check 'i + 1' if 'i' is less than the last index
		    if (i < waypoints.Length - 1) 
		    {
			if (waypoints[i + 1] != null)
			{
			    Gizmos.DrawLine(waypoints[i].position, waypoints[i + 1].position);
			}
		    }
		}
	    }
	}
}
